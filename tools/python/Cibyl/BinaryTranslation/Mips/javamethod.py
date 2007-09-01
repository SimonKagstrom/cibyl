######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:      javamethod.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Java method stuff
##
## $Id: javamethod.py 14162 2007-03-11 14:00:55Z ska $
##
######################################################################
from Cibyl.BinaryTranslation import bytecode, register
from Cibyl.BinaryTranslation.Optimization.Mips import memoryRegisters
from Cibyl.BinaryTranslation.Optimization.Mips import mult
from Cibyl import config
import mips, instruction
from sets import Set
import struct

from Cibyl.BinaryTranslation.codeblock import CodeBlock

class JavaMethod(CodeBlock):
    """
    One method per function Java mehtod.
    """
    def __init__(self, controller, functions):
	self.functions = functions
	self.controller = controller
        self.labels = {}
        self.instructions = []
        self.exceptionHandlers = []
        self.returnAddresses = {}

        for fn in self.functions:
            fn.setJavaMethod(self)
            for key, item in fn.labels.iteritems():
                assert(not self.labels.has_key(key))
                self.labels[key] = item
            self.instructions = self.instructions + fn.instructions

            if fn.address == self.controller.elf.getEntryPoint():
                self.name = "start"
                self.methodAccess = "public"
            else:
                sym = self.controller.elf.getSymbolByAddress(fn.address)
                if sym.localLinkage():
                    self.name = fn.name +  "_%x" % (fn.address)
                else:
                    self.name = fn.name
                self.methodAccess = "private"
	self.bc = bytecode.ByteCodeGenerator(self.controller)
	self.rh = register.RegisterHandler(self.controller, self.bc)

        useTracing = False
        for fn in self.functions:
            if fn.useTracing:
                useTracing = True
                break

	CodeBlock.__init__(self, self.controller, self.instructions, self.labels, useTracing, setupRegisters=True)

	# Each label and instruction belongs to one java method
	for lab in self.labels.values():
	    if lab.address >= self.address and lab.address <= self.address + self.size:
		lab.setJavaMethod(self)

	self.cleanupRegs = Set()

	# For non-leafs, we always pass all registers
        i = 0
        for fn in self.functions:
            if not fn.isLeaf() or fn.address == self.controller.elf.getEntryPoint():
                self.addSourceRegister(mips.R_SP)
                self.addSourceRegister(mips.R_A0)
                self.addSourceRegister(mips.R_A1)
                self.addSourceRegister(mips.R_A2)
                self.addSourceRegister(mips.R_A3)
                self.addDestinationRegister(mips.R_V0)
                self.addDestinationRegister(mips.R_V1)
            fn.setIndex(i)
            i = i + 1

	self.argumentRegisters = [mips.R_SP, mips.R_A0, mips.R_A1, mips.R_A2, mips.R_A3]
	if not config.debug:
	    self.argumentRegisters = list(self.usedRegisters.intersection( [mips.R_SP, mips.R_A0, mips.R_A1, mips.R_A2, mips.R_A3] ))
	self.argumentRegisters.sort()
	# Always place sp first
	if mips.R_SP in self.argumentRegisters:
	    self.argumentRegisters.remove(mips.R_SP)
	    self.argumentRegisters = [mips.R_SP] + self.argumentRegisters
        # And add the function address argument last if we have multiple functions / method
        if self.hasMultipleFunctions():
            self.argumentRegisters = self.argumentRegisters + [mips.R_FNA]

    def setJavaClass(self, javaClass):
        self.javaClass = javaClass

    def getJavaMethodName(self):
	return self.name + "(" + 'I'*len(self.getRegistersToPass()) + ")" + self.getJavaReturnType()

    def getJavaReturnType(self):
	if mips.R_V0 in self.usedRegisters:
	    return "I"
	else:
	    return "V"

    def getFunction(self, address):
        for fn in self.functions:
            if fn.address == address:
                return fn
        abort("No such function")

    def hasMultipleFunctions(self):
        return len(self.functions) > 1

    def invoke(self, address):
        "Invoke this method (e.g., through a JAL instruction)"
        self.bc.invokestatic( "CompiledProgram/" + self.getJavaMethodName() )

    def addReturnAddress(self, address):
        try:
            return self.returnAddresses[address]
        except:
            curIdx = len(self.returnAddresses)
            self.returnAddresses[address] = curIdx
            return self.returnAddresses[address]

    def addExceptionHandler(self, startAddress, endAddress):
        self.exceptionHandlers.append( (startAddress, endAddress) )
        return "L_EXH_%x_to_%x" % (startAddress, endAddress)

    def getRegistersToPass(self):
	"Get the registers to pass to this function"
	if not config.doRegisterScheduling:
            out = [mips.R_SP, mips.R_A0, mips.R_A1, mips.R_A2, mips.R_A3]
            if self.hasMultipleFunctions():
                return out + [mips.R_FNA]
	    return out
	return self.argumentRegisters

    def clobbersReg(self, reg):
	return reg in self.destinationRegisters

    def getNumberOfLocals(self):
	"Return the number of used registers by this function"
	if not config.doRegisterScheduling:
	    return mips.N_REGS
	return len(self.usedRegisters)


    def generateLookupTable(self):
	out = []
	for label in self.labels.values():
	    # Don't include the actual function address
            found = False
            for fn in self.functions:
                if label.address == fn.address:
                    found = True
            if found:
                continue
	    if label.inJumpTab and label.address > self.address and label.address < self.address + self.size:
		cur = (label.address, str(label))
		out.append(cur)
	return out

    def fixup(self):
	"""This method constructs a list of clobbered registers
	(depending on the functions this function calls) and also adds
	registers to zero in the function."""
        # First run the fixup on the functions, which will optimize
        # away one stack stores
        for fn in self.functions:
            fn.fixup()

	if config.doMemoryRegisterOptimization:
            for fn in self.functions:
                for bb in fn.basicBlocks:
                    memoryRegisters.run(bb)
                    self.addDestinationRegisterSet(bb.destinationRegisters)
                    self.addSourceRegisterSet(bb.sourceRegisters)

	if config.doMultOptimization:
            for fn in self.functions:
                mult.run(fn)

	# Add v1 to the list of clobbered registers if the destination
	# method clobbers V1
        self.maxOperandStack = 0
	for insn in self.instructions:
	    insn.setJavaMethod(self)
            if isinstance(insn, instruction.Jal) and insn.dstAddress not in map(lambda fn : fn.address, self.functions):
		otherMethod = self.controller.lookupJavaMethod(insn.dstAddress)
		if otherMethod.clobbersReg(mips.R_V1):
		    self.addDestinationRegister(mips.R_V1)
		if otherMethod.clobbersReg(mips.R_V0):
		    self.addDestinationRegister(mips.R_V0)
	    elif isinstance(insn, instruction.Jalr):
		self.addDestinationRegister(mips.R_V1)
		self.addDestinationRegister(mips.R_V0)
            operandStackSize = insn.maxOperandStackHeight()
            if insn.delayed:
                operandStackSize = operandStackSize + insn.delayed.maxOperandStackHeight()
            if insn.prefix:
                operandStackSize = operandStackSize + insn.prefix.maxOperandStackHeight()

            if operandStackSize > self.maxOperandStack:
                self.maxOperandStack = operandStackSize


	# Somewhat conservative estimate of registers to zero - all
	# used registers which are read before written to in the first
	# basic block - or are not used there - are placed in the
	# cleanup list
        skipRegisters = Set(self.argumentRegisters + mips.memoryAddressRegisters + [mips.R_HI, mips.R_LO, mips.R_ZERO, mips.R_RA])
        checkRegisters = self.usedRegisters - skipRegisters
        for fn in self.functions:
            bb0 = fn.basicBlocks[0]
            for r in checkRegisters:
                if bb0.registerReadBeforeWritten(r):
                    self.cleanupRegs.add(r)
        if self.hasMultipleFunctions():
            self.cleanupRegs.add(mips.R_RA)


    def compile(self):
	"""Compile this method. The calling convention looks as
	follows:

	int method(int sp, int a0, int a1, int a2, int a3);

	The return result is register v0. Register v1 is placed in a
	static class-variable and is assigned after calls.
	"""
	self.controller.emit(".method %s static %s" % (self.methodAccess, self.getJavaMethodName()) )
        if not config.operandStackLimit:
            self.controller.emit(".limit stack %d" % (self.maxOperandStack))
        else:
            self.controller.emit(".limit stack %d" % (config.operandStackLimit))

	# Generate a register mapping for this method and the registers to pass to the method
        registerMapping = { self.functions[0].address : register.reg2local }
        maxLocals = len(register.reg2local)
	if config.doRegisterScheduling:
	    registerMapping, maxLocals = register.generateRegisterMapping(self.functions, self.usedRegisters,
                                                                          self.argumentRegisters, self.registerUseCount)
            # Default to the first (doesn't matter which)
            register.reg2local = registerMapping[ self.functions[0].address ]
        self.controller.emit(".limit locals %d" % maxLocals)

        # Not really needed - if we have config.debug, we will
        # only have one function per java method
        for fn in self.functions:
            for reg, local in registerMapping[fn.address].iteritems():
                if (reg not in self.usedRegisters and reg not in self.argumentRegisters) or reg in register.staticRegs:
                    continue
                if config.debug:
                    self.controller.emit(".var %2d is %s I from METHOD_START to METHOD_END" % (local, mips.registerNames[reg]))
                else:
                    self.controller.emit("; local %2d is register %s" % (local, mips.registerNames[reg]))

	for reg in self.cleanupRegs:
            localsToZero = Set()
            # Add all possible mappings to zero
            for fn in self.functions:
                mapping = registerMapping[fn.address]
                if mapping.has_key(reg):
                    localsToZero.add(mapping[reg])
            for local in localsToZero:
                if reg == mips.R_RA:
                    self.bc.pushConst(-1)
                else:
                    self.bc.pushConst(0)
                self.bc.istore(local)

	if config.traceFunctionCalls:
	    self.bc.ldc("0x%08x: %s" % (self.address, self.name))
	    self.bc.invokestatic("CRunTime/emitFunctionEnterTrace(Ljava/lang/String;)V")

	self.controller.emit("METHOD_START:")
        if self.hasMultipleFunctions():
            switchTuples = []
            for fn in self.functions:
                switchTuples.append( (fn.getIndex(), "L_%x" % fn.address) )
            self.rh.pushRegister(mips.R_FNA)
            self.bc.tableswitch(switchTuples, "__CIBYL_function_return")

	# Compile this function
        for fn in self.functions:
            register.reg2local = registerMapping[ fn.address ]
            fn.compile()

	# Emit the local jumptable
	lookuptab = self.generateLookupTable()
	if lookuptab:
	    self.controller.emit("__CIBYL_local_jumptab:")
	    self.bc.lookupswitch(lookuptab, "__CIBYL_function_return")

        # Handle try/catch pairs
        for eh in self.exceptionHandlers:
            start, end = eh
            self.bc.emit("L_EXH_%x_to_%x:" % (start, end))
            # Register the object
            self.bc.invokestatic("CRunTime/registerObject(Ljava/lang/Object;)I")

            self.rh.pushRegister(mips.R_ECB)
            self.bc.emit("swap")
            # Compile the JALR instruction
            self.rh.pushRegister(mips.R_SP)
            self.bc.emit("swap")
            self.rh.pushRegister(mips.R_EAR)
            self.bc.pushConst(0)
            self.bc.pushConst(0)
            self.bc.invokestatic("CompiledProgram/__CIBYL_global_jumptab(IIIIII)I")
            self.bc.pop()
            self.bc.goto( end )

	# Emit the return statement
	self.controller.emit("__CIBYL_function_return:")
	if config.traceFunctionCalls:
	    self.bc.ldc("")
	    self.bc.invokestatic("CRunTime/emitFunctionExitTrace(Ljava/lang/String;)V")

        if self.hasMultipleFunctions():
            self.rh.pushRegister(mips.R_RA)
            table = []
            for address, key in self.returnAddresses.iteritems():
                table.append( (key, str(self.labels[address])) )
            self.bc.tableswitch(table, "__CIBYL_non_local_return")
            self.controller.emit("__CIBYL_non_local_return:")

	if self.clobbersReg(mips.R_V1):
	    self.rh.pushRegister(mips.R_V1)
	    self.bc.putstatic("CRunTime/saved_v1 I")

	if self.getJavaReturnType() == "I":
	    self.rh.pushRegister(mips.R_V0)
	    self.bc.ireturn()
	else:
	    self.bc.emit_return()

	self.controller.emit("METHOD_END:")
	self.controller.emit(".end method")


class GlobalJumptabMethod(CodeBlock):
    def __init__(self, controller, functions):
	self.controller = controller
	self.name = "__CIBYL_global_jumptab(IIIIII)I"
	self.bc = bytecode.ByteCodeGenerator(self.controller)
	self.rh = register.RegisterHandler(self.controller, self.bc)

	CodeBlock.__init__(self, self.controller, [], [], False)
        self.functions = functions
        self.address = 0
        self.size = 0

    def cleanupFunctions(self, functions):
        if not config.doOptimizeIndirectCalls:
            return functions

        out = []

        # Lookup relocations to the text segment which are split
        # between instructions, i.e., things like lui/addiu
        # pairs. This catches the case when there is a static function
        # which is called through a function pointer
        textRelocs = self.controller.elf.getRelocationsBySection(".text")
        textRelocs = [ rel for rel in textRelocs if rel.value == self.controller.elf.getSectionAddress(".text") and rel.type in ("R_MIPS_HI16", "R_MIPS_LO16") ]

        curBasicBlock = None
        relocsToCheck = []
        for cur in textRelocs:
            if not self.controller.lookupFunction(cur.offset):
                continue
            bb = self.controller.getInstruction(cur.offset).getBasicBlock()
            # Was this pruned?
            if not self.controller.lookupFunction(cur.offset):
                continue
            if curBasicBlock != None and curBasicBlock.address != bb.address:
                relocFunctions = self.getFunctionsInText(relocsToCheck)
                for function in relocFunctions:
                    if function not in out:
                        out.append(function)
                curBasicBlock = bb
                relocsToCheck = []
            relocsToCheck.append(cur)
        else:
            relocFunctions = self.getFunctionsInText(relocsToCheck)
            for function in relocFunctions:
                if function not in out:
                    out.append(function)

        # Lookup relocations in the data segment
        dataSection = self.controller.elf.getSectionContents(".data") + self.controller.elf.getSectionContents(".rodata")
        for i in range(0, len(dataSection), 4):
            # Unpack this word
	    data = dataSection[i : i + 4]
	    address = struct.unpack(">L", data)[0]
            function = self.controller.lookupFunction(address)
            if function and function not in out:
                out.append(function)

        # Get all the direct relocations
        for function in functions:
            relocs = self.controller.elf.getRelocationsByTargetAddress(function.address)
            for r in relocs:
                if r.type in ("R_MIPS_HI16", "R_MIPS_LO16", "R_MIPS_32"):
                    if function in out:
                        continue
                    out.append(function)
            if self.controller.elf.isConstructor(function.address) or self.controller.elf.isDestructor(function.address):
                if function not in out:
                    out.append(function)
        return out

    def getFunctionsInText(self, relocsToCheck):
        out = []
        # Try to combine all low and high parts
        for r in [ rel for rel in relocsToCheck if rel.type == "R_MIPS_HI16" ]:
            for s in [ rel for rel in relocsToCheck if rel.type == "R_MIPS_LO16" ]:
                insn_high = self.controller.getInstruction(r.offset)
                insn_low = self.controller.getInstruction(s.offset)
                address = ((insn_high.extra & 0xffff) << 16) + insn_low.extra
                fn = self.controller.lookupFunction(address)
                if fn:
                    if config.verbose: print "found", fn.name
                    if fn not in out:
                        out.append(fn)
                elif config.verbose: print "Didn't find for %x" % (address)
        return out


    def generateLookupTable(self):
	out = []
	for fn in self.functions:
	    cur = (fn.address, "lab_%s_%x" % (fn.name, fn.address))
	    out.append(cur)
	return out

    def setJavaClass(self, javaClass):
        self.javaClass = javaClass

    def fixup(self):
        # Put all methods which have lo16/hi16 or 32-bit relocations or are within
        # the constructors/destructors sections
        self.functions = self.cleanupFunctions(self.functions)

    def compile(self):
	self.controller.emit(".method public static %s" % (self.name))
        if not config.operandStackLimit:
            self.controller.emit(".limit stack 14")
        else:
            self.controller.emit(".limit stack %d" % (config.operandStackLimit))
	self.controller.emit(".limit locals 8")

	# This is the register allocation for the global jumptab
	reg2local = {
	    mips.namesToRegisters["sp"] : 1,
	    mips.namesToRegisters["a0"] : 2,
	    mips.namesToRegisters["a1"] : 3,
	    mips.namesToRegisters["a2"] : 4,
	    mips.namesToRegisters["a3"] : 5,
	}

	if config.traceFunctionCalls:
	    self.bc.ldc("0x%08x: %s" % (self.address, self.name))
	    self.bc.invokestatic("CRunTime/emitFunctionEnterTrace(Ljava/lang/String;)V")

	self.bc.iload(0) # First argument is the method to call
	self.bc.pushConst(0)
	self.bc.istore(7)

	lookuptab = self.generateLookupTable()
	self.bc.lookupswitch(lookuptab, "error")

	for fn in self.functions:
            method = fn.getJavaMethod()
	    self.controller.emit("lab_%s_%x:" % (fn.name, fn.address))
	    for r in method.getRegistersToPass():
                if r == mips.R_FNA:
                    self.bc.pushConst( fn.getIndex() )
                else:
                    self.bc.iload( reg2local[r] )
            # Invoke the method.
            method.invoke(0)
	    if method.getJavaReturnType() == "I":
		self.bc.istore(7)
	    self.bc.goto("out")

 	self.controller.emit("error:")
	self.bc.ldc("CIBYL ERROR: cannot find the address ")
	self.bc.iload(0)
	self.bc.invokestatic("CRunTime/abortWithAddress(Ljava/lang/String;I)V")

 	self.controller.emit("out:")
	if config.traceFunctionCalls:
	    self.bc.ldc("")
	    self.bc.invokestatic("CRunTime/emitFunctionExitTrace(Ljava/lang/String;)V")
	self.bc.iload(7)
	self.bc.ireturn()
 	self.controller.emit(".end method")
