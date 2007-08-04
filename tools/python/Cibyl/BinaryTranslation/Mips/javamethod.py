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

	CodeBlock.__init__(self, self.controller, self.instructions, self.labels, useTracing)

	# Each label and instruction belongs to one java method
	for lab in self.labels.values():
	    if lab.address >= self.address and lab.address <= self.address + self.size:
		lab.setJavaMethod(self)

	self.cleanupRegs = Set()

	# For non-leafs, we always pass all registers
        for fn in self.functions:
            if not fn.isLeaf() or fn.address == self.controller.elf.getEntryPoint():
                self.addSourceRegister(mips.R_SP)
                self.addSourceRegister(mips.R_A0)
                self.addSourceRegister(mips.R_A1)
                self.addSourceRegister(mips.R_A2)
                self.addSourceRegister(mips.R_A3)
                self.addDestinationRegister(mips.R_V0)
                self.addDestinationRegister(mips.R_V1)

	for insn in self.instructions:
            # If the destination is within the same method, call it
            # with a goto, but not if it is a recursive function call
            if isinstance(insn, instruction.Jal) and \
            insn.dstAddress in map(lambda fn : fn.address and fn.address != insn.dstAddress, self.functions):
                insn.compile = insn.compileJump
	    insn.setJavaMethod(self)

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

	# Perform skip stack store optimization. We actually always do
	# this since we that way avoid zeroing registers unecessary
	if not config.debug:
	    self.doSkipStackStoreOptimization()

	if config.doMemoryRegisterOptimization:
            for fn in self.functions:
                for bb in fn.basicBlocks:
                    memoryRegisters.run(bb)
                    self.addDestinationRegisterSet(bb.destinationRegisters)
                    self.addSourceRegisterSet(bb.sourceRegisters)

	if config.doMultOptimization:
            for fn in self.functions:
                mult.run(fn)

    def getJavaMethodName(self):
	return self.name + "(" + 'I'*len(self.getRegistersToPass()) + ")" + self.getJavaReturnType()

    def getJavaReturnType(self):
	if mips.R_V0 in self.usedRegisters:
	    return "I"
	else:
	    return "V"

    def hasMultipleFunctions(self):
        return len(self.functions) > 1

    def invoke(self, address):
        "Invoke this method (e.g., through a JAL instruction)"
        self.bc.invokestatic( "CompiledProgram/" + self.getJavaMethodName() )

    def addExceptionHandler(self, startAddress, endAddress):
        self.exceptionHandlers.append( (startAddress, endAddress) )
        return "EXH_%x_to_%x" % (startAddress, endAddress)

    def getRegistersToPass(self):
	"Get the registers to pass to this function"
	if not config.doRegisterScheduling:
            out = [mips.R_SP, mips.R_A0, mips.R_A1, mips.R_A2, mips.R_A3]
            if self.hasMultipleFunctions():
                return out + [mips.R_FNA]
	    return out
	return self.argumentRegisters

    def doSkipStackStoreOptimization(self):
	"""Optimization to skip the stack saving and restoring since
	these are not actually needed with Cibyl (registers in local
	java variables)"""

	def instructionIsStackStore(insn):
	    return isinstance(insn, instruction.Sw) and insn.rt in mips.callerSavedRegisters and insn.rs == mips.R_SP

	def instructionIsStackLoad(insn):
	    return isinstance(insn, instruction.Lw) and insn.rt in mips.callerSavedRegisters and insn.rs == mips.R_SP

        for fn in self.functions:
            for insn in fn.basicBlocks[0].instructions:
                if instructionIsStackStore(insn):
                    if config.verbose: print "Removing", insn
                    insn.nullify()

        returnBasicBlocks = []
        for fn in self.functions:
            returnBasicBlocks = returnBasicBlocks + fn.getReturnBasicBlocks()
	for bb in returnBasicBlocks:
	    for insn in bb.instructions:
		# Validate that there is no use of the saved registers
		for dst in self.destinationRegisters:
		    if dst in mips.callerSavedRegisters and not instructionIsStackLoad(insn):
			return

	    for insn in bb.instructions:
		if instructionIsStackLoad(insn):
		    if config.verbose: print "Removing", insn
		    insn.nullify()

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
	    if label.inJumpTab and label.address > self.address and label.address < self.address + self.size:
		cur = (label.address, str(label))
		out.append(cur)
	return out

    def fixup(self):
	"""This method constructs a list of clobbered registers
	(depending on the functions this function calls) and also adds
	registers to zero in the function."""
	# Add v1 to the list of clobbered registers if the destination
	# method clobbers V1
	for insn in self.instructions:
            if isinstance(insn, instruction.Jal) and insn.dstAddress not in map(lambda fn : fn.address, self.functions):
		otherMethod = self.controller.lookupJavaMethod(insn.dstAddress)
		if otherMethod.clobbersReg(mips.R_V1):
		    self.addDestinationRegister(mips.R_V1)
		if otherMethod.clobbersReg(mips.R_V0):
		    self.addDestinationRegister(mips.R_V0)
	    elif isinstance(insn, instruction.Jalr):
		self.addDestinationRegister(mips.R_V1)
		self.addDestinationRegister(mips.R_V0)

	# Somewhat conservative estimate of registers to zero - all
	# used registers which are not used as destinations in the
	# first basic block. This _requires_ that the s-register stack
	# removal optimization is turned on.
        for fn in self.functions:
            bb0 = fn.basicBlocks[0]
            for insn in bb0.instructions:
                for r in self.usedRegisters:
                    if config.debug:
                        if not register.staticRegs.has_key(r) and r not in self.argumentRegisters + [mips.R_ZERO]:
                            self.cleanupRegs.add(r)
                    elif (r not in self.argumentRegisters + [mips.R_HI, mips.R_LO, mips.R_ZERO, mips.R_RA]) and not bb0.registerWrittenToBefore(insn, r):
                        self.cleanupRegs.add(r)


    def compile(self):
	"""Compile this method. The calling convention looks as
	follows:

	int method(int sp, int a0, int a1, int a2, int a3);

	The return result is register v0. Register v1 is placed in a
	static class-variable and is assigned after calls.
	"""
	self.controller.emit(".method %s static %s" % (self.methodAccess, self.getJavaMethodName()) )
        maxOperandStack = 0
        if not config.operandStackLimit:
            for insn in self.instructions:
                operandStackSize = insn.maxOperandStackHeight()
                if insn.delayed:
                    operandStackSize = operandStackSize + insn.delayed.maxOperandStackHeight()
                if insn.prefix:
                    operandStackSize = operandStackSize + insn.prefix.maxOperandStackHeight()

                if operandStackSize > maxOperandStack:
                    maxOperandStack = operandStackSize
            self.controller.emit(".limit stack %d" % (maxOperandStack))
        else:
            self.controller.emit(".limit stack %d" % (config.operandStackLimit))
	self.controller.emit(".limit locals %d" % self.getNumberOfLocals())

	# Generate a register mapping for this method and the registers to pass to the method
	if config.doRegisterScheduling:
	    registerMapping = register.generateRegisterMapping(self.usedRegisters, self.argumentRegisters, self.registerUseCount)

	    register.reg2local = registerMapping
            if config.debug:
                for reg, local in registerMapping.iteritems():
                    self.controller.emit(".var %2d is %s I from METHOD_START to METHOD_END" % (local, mips.registerNames[reg]))
	else:
            if config.debug:
                for reg, local in register.reg2local.iteritems():
                    if (reg not in self.usedRegisters and reg not in self.argumentRegisters) or reg in register.staticRegs:
                        continue
                    self.controller.emit(".var %2d is %s I from METHOD_START to METHOD_END" % (local, mips.registerNames[reg]))

	for reg in self.cleanupRegs:
	    self.bc.pushConst(0)
	    self.rh.popToRegister(reg)

	if config.traceFunctionCalls:
	    self.bc.ldc("0x%08x: %s" % (self.address, self.name))
	    self.bc.invokestatic("CRunTime/emitFunctionEnterTrace(Ljava/lang/String;)V")

	self.controller.emit("METHOD_START:")
        if self.hasMultipleFunctions():
            switchTuples = []
            for fn in self.functions:
                switchTuples.append( (fn.address, "L_%x" % fn.address) )
            self.rh.pushRegister(mips.R_FNA)
            self.bc.lookupswitch(switchTuples, "__CIBYL_function_return")

	# Compile this function
        for fn in self.functions:
            fn.compile()

	# Emit the local jumptable
	lookuptab = self.generateLookupTable()
	if lookuptab:
	    self.controller.emit("__CIBYL_local_jumptab:")
	    self.bc.lookupswitch(lookuptab, "__CIBYL_function_return")

        # Handle try/catch pairs
        for eh in self.exceptionHandlers:
            start, end = eh
            self.bc.emit("EXH_%x_to_%x:" % (start, end))
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

	# Put all methods which have lo16/hi16 or 32-bit relocations or are within
        # the constructors/destructors sections
        self.functions = self.cleanupMethods(functions)

	self.bc = bytecode.ByteCodeGenerator(self.controller)
	self.rh = register.RegisterHandler(self.controller, self.bc)

	CodeBlock.__init__(self, self.controller, [], [], False)

    def cleanupMethods(self, methods):
        if not config.doOptimizeIndirectCalls:
            return methods

        out = []

        # Lookup relocations to the text segment which are split
        # between instructions, i.e., things like lui/addiu
        # pairs. This catches the case when there is a static method
        # which is called through a function pointer
        textRelocs = self.controller.elf.getRelocationsBySection(".text")
        textRelocs = [ rel for rel in textRelocs if rel.value == self.controller.elf.getSectionAddress(".text") and rel.type in ("R_MIPS_HI16", "R_MIPS_LO16") ]

        curBasicBlock = None
        relocsToCheck = []
        for cur in textRelocs:
            bb = self.controller.getInstruction(cur.offset).getBasicBlock()
            if curBasicBlock != None and curBasicBlock.address != bb.address:
                relocMethods = self.getMethodsInText(relocsToCheck)
                for method in relocMethods:
                    if method not in out:
                        out.append(method)
                curBasicBlock = bb
                relocsToCheck = []
            relocsToCheck.append(cur)
        else:
            relocMethods = self.getMethodsInText(relocsToCheck)
            for method in relocMethods:
                if method not in out:
                    out.append(method)

        # Lookup relocations in the data segment
        dataSection = self.controller.elf.getSectionContents(".data") + self.controller.elf.getSectionContents(".rodata")
        for i in range(0, len(dataSection), 4):
            # Unpack this word
	    data = dataSection[i : i + 4]
	    address = struct.unpack(">L", data)[0]
            method = self.controller.lookupJavaMethod(address)
            if method and method not in out:
                out.append(method)

        # Get all the direct relocations
        for method in methods:
            for fn in method.functions:
                relocs = self.controller.elf.getRelocationsByTargetAddress(fn.address)
                for r in relocs:
                    if r.type in ("R_MIPS_HI16", "R_MIPS_LO16", "R_MIPS_32"):
                        if method in out:
                            continue
                        out.append(method)
                if self.controller.elf.isConstructor(fn.address) or self.controller.elf.isDestructor(fn.address):
                    if method not in out:
                        out.append(method)
        return out

    def getMethodsInText(self, relocsToCheck):
        out = []
        # Try to combine all low and high parts
        for r in [ rel for rel in relocsToCheck if rel.type == "R_MIPS_HI16" ]:
            for s in [ rel for rel in relocsToCheck if rel.type == "R_MIPS_LO16" ]:
                insn_high = self.controller.getInstruction(r.offset)
                insn_low = self.controller.getInstruction(s.offset)
                address = ((insn_high.extra & 0xffff) << 16) + insn_low.extra
                method = self.controller.lookupJavaMethod(address)
                if method:
                    if config.verbose: print "found", method.name
                    if method not in out:
                        out.append(method)
                elif config.verbose: print "Didn't find for %x" % (address)
        return out


    def generateLookupTable(self):
	out = []
	for fn in self.functions:
	    cur = (fn.address, "lab_" + fn.name)
	    out.append(cur)
	return out

    def fixup(self):
	pass

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
	    self.controller.emit("lab_" + fn.name + ":")
	    for r in method.getRegistersToPass():
                if r == mips.R_FNA:
                    self.bc.pushConst(fn.address)
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
