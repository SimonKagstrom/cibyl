######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:	  javamethod.py
## Author:		Simon Kagstrom <ska@bth.se>
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

			self.name = fn.name
			self.methodAccess = "public"
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
			self.addSourceRegister(mips.R_MEM)
			self.addDestinationRegister(mips.R_MEM)
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
		self.usesMemoryInstructions = False

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

	def invoke(self, address=0):
		"Invoke this method (e.g., through a JAL instruction)"
		self.bc.invokestatic( self.javaClass.name + "/" + self.getJavaMethodName() )

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
                        elif isinstance(insn, instruction.MemoryAccess):
				self.usesMemoryInstructions = True
			operandStackSize = insn.maxOperandStackHeight()
			if insn.delayed:
				operandStackSize = operandStackSize + insn.delayed.maxOperandStackHeight()
				if isinstance(insn.delayed, instruction.MemoryAccess):
					self.usesMemoryInstructions = True
			if insn.prefix:
				operandStackSize = operandStackSize + insn.prefix.maxOperandStackHeight()
				if isinstance(insn.prefix, instruction.MemoryAccess):
					self.usesMemoryInstructions = True

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
		if len(fn.basicBlocks) > 1:
			for bb in fn.basicBlocks[1 : -1]:
				if bb.raUsed(mips.R_RA):
					self.cleanupRegs.add(mips.R_RA)
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

		self.cleanupRegs.remove(mips.R_MEM)

		for reg in self.cleanupRegs:
			localsToZero = Set()
			# Add all possible mappings to zero
			for fn in self.functions:
				mapping = registerMapping[fn.address]
				if mapping.has_key(reg) and not register.regIsStatic(reg):
					localsToZero.add(mapping[reg])
			for local in localsToZero:
				if reg == mips.R_RA and self.hasMultipleFunctions():
					self.bc.pushConst(-1)
				else:
					self.bc.pushConst(0)
				self.bc.istore(local)

		if config.aloadMemory and self.usesMemoryInstructions:
			self.bc.getstatic("CRunTime/memory [I")
			self.rh.popToRegister(mips.R_MEM)

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
			self.controller.getGlobalCallTableMethod().invoke()
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


class GlobalJavaCallTableMethod(JavaMethod):
	def __init__(self, controller, functions):
		self.controller = controller
		self.name = "call"
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
			if self.controller.lookupFunction(cur.offset):
				bb = self.controller.getInstruction(cur.offset).getBasicBlock()
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
				# Was this pruned?
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
				try:
					insn_high = self.controller.getInstruction(r.offset)
					insn_low = self.controller.getInstruction(s.offset)
				except:
					continue
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

	def fixup(self):
		# Put all methods which have lo16/hi16 or 32-bit relocations or are within
		# the constructors/destructors sections
		self.functions = self.cleanupFunctions(self.functions)

	def getJavaMethodName(self):
		return self.name + "(IIIIII)I"

	def getRegistersToPass(self):
		return [mips.R_SP, mips.R_A0, mips.R_A1, mips.R_A2, mips.R_A3]

	def getSize(self):
		"Approximate the size of this method"
		return len(self.functions) * 10

	def getJavaReturnType(self):
		return "I"

	def compile(self):
		reg2local = {
					mips.namesToRegisters["sp"] : "sp",
					mips.namesToRegisters["a0"] : "a0",
					mips.namesToRegisters["a1"] : "a1",
					mips.namesToRegisters["a2"] : "a2",
					mips.namesToRegisters["a3"] : "a3",
					}
		size = len(self.functions) / config.callTableHierarchy

		self.controller.emit("public static final int %s(int address, int sp, int a0, int a1, int a2, int a3)  throws Exception {" % self.name)

		# These *must* be sorted
		self.functions.sort()

		if config.callTableHierarchy > 1:
			for i in range(0, config.callTableHierarchy):
				if i == config.callTableHierarchy-1:
					self.controller.emit("else")
				else:
					first = self.functions[ i * size ]
					last = self.functions[ (i+1) * size ] # Don't dereference the last if it's outside
					self.controller.emit("if (address >= 0x%x && address < 0x%x)" % (first.address, last.address))
				self.controller.emit("  return %s%d(address, sp, a0,a1,a2,a3);" % (self.name, i))
			self.controller.emit("}")

		for i in range(0, config.callTableHierarchy):
			if config.callTableHierarchy > 1:
				self.controller.emit("private static final int %s%d(int address, int sp, int a0, int a1, int a2, int a3) throws Exception {" % (self.name, i))
			self.controller.emit("int v0 = 0;");
			self.controller.emit("switch(address) {")
			fns = self.functions[ i*size : (i+1) * size ]
			if i == config.callTableHierarchy-1:
				fns = self.functions[ i*size : ]
			for fn in fns:
				method = fn.getJavaMethod()
				self.controller.emit("case 0x%x: " % fn.address)
				prefix = ""
				if method.getJavaReturnType() == "I":
					prefix = "v0 = "
				self.controller.emit("%s%s.%s(" % (prefix, method.javaClass.name, method.name)) # method.javaClass.name
				count = 0
				for r in method.getRegistersToPass():
					count = count + 1
					suffix =  ","
					if count == len(method.getRegistersToPass()):
						suffix = ""
					if r == mips.R_FNA:
						self.controller.emit(("0x%x" % fn.getIndex()) + suffix)
					else:
						self.controller.emit( reg2local[r] + suffix)
				self.controller.emit("); break;")
			self.controller.emit('default: throw new Exception("Call to unknown location " + Integer.toHexString(address));}')
			self.controller.emit("return v0;")
			self.controller.emit("}")
