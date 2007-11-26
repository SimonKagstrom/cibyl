######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:	  register.py
## Author:		Simon Kagstrom <ska@bth.se>
## Description:   Cibyl register handling
##
## $Id: register.py 13453 2007-02-05 16:28:37Z ska $
##
######################################################################
from Cibyl.BinaryTranslation.Mips import mips
from Cibyl import config
from sets import Set

reg2local = {
	mips.namesToRegisters["sp"] :  0,
	mips.namesToRegisters["a0"] :  1,
	mips.namesToRegisters["a1"] :  2,
	mips.namesToRegisters["a2"] :  3,
	mips.namesToRegisters["a3"] :  4,
	mips.namesToRegisters["v0"] :  5,
	mips.namesToRegisters["v1"] :  6,
	mips.namesToRegisters["t0"] :  7,
	mips.namesToRegisters["t1"] :  8,
	mips.namesToRegisters["t2"] :  9,
	mips.namesToRegisters["t3"] : 10,
	mips.namesToRegisters["t4"] : 11,
	mips.namesToRegisters["t5"] : 12,
	mips.namesToRegisters["t6"] : 13,
	mips.namesToRegisters["t7"] : 14,
	mips.namesToRegisters["t8"] : 15,
	mips.namesToRegisters["t9"] : 16,
	mips.namesToRegisters["s0"] : 17,
	mips.namesToRegisters["s1"] : 18,
	mips.namesToRegisters["s2"] : 19,
	mips.namesToRegisters["s3"] : 20,
	mips.namesToRegisters["s4"] : 21,
	mips.namesToRegisters["s5"] : 22,
	mips.namesToRegisters["s6"] : 23,
	mips.namesToRegisters["s7"] : 24,
	mips.namesToRegisters["s8"] : 25,
	mips.namesToRegisters["at"] : 26,
	mips.namesToRegisters["ra"] : 27,
	mips.namesToRegisters["cpc"]: 28,
	mips.namesToRegisters["cm0"]: 29,
	mips.namesToRegisters["cm1"]: 30,
	mips.namesToRegisters["cm2"]: 31,
	mips.namesToRegisters["cm3"]: 32,
	mips.namesToRegisters["cm4"]: 33,
	mips.namesToRegisters["cm5"]: 34,
	mips.namesToRegisters["cm6"]: 35,
	mips.namesToRegisters["cm7"]: 36,
	mips.namesToRegisters["cm8"]: 37,
	mips.namesToRegisters["gp"] : 38,
	mips.namesToRegisters["hi"] : 39,
	mips.namesToRegisters["lo"] : 40,
	mips.namesToRegisters["ecb"] : 41,
	mips.namesToRegisters["ear"] : 42,
}

staticRegs = {
	mips.namesToRegisters["ra"] : "CRunTime/ra I", # For the debug target
}

def regIsStatic(reg):
	# Special-case for multi-function-methods (need to store RA for
	# internal consumption)
	if not config.debug and reg == mips.R_RA:
		return False
	return staticRegs.has_key(reg)

def sortedDictValues(indict):
	"Return the items in indict in a sorted list. Ugly..."
	out = []
	indict = dict(indict)

	while indict != {}:
		hi = -1
		hkey = -1
		for k, v in indict.iteritems():
			if v > hi:
				hi = v
				hkey = k
		out.append( k )
		indict.pop(k)
	return out

def generateRegisterMapping(functions, usedRegisters, argumentRegisters, registerUseCount):
	out = {}
	mapping = {}
	count = 0

	usedRegisters = Set(usedRegisters)
	registerUseCount = dict(registerUseCount)
	argumentRegisters = list(argumentRegisters)

	# First assign the arguments
	for reg in argumentRegisters:
		mapping[ reg ] = count
		count = count + 1
		usedRegisters.discard(reg)

	# Discard some registers
	for reg in sortedDictValues(registerUseCount):
		if reg == mips.R_ZERO or regIsStatic(reg):
			usedRegisters.discard(reg)

	# Then the other registers, sans caller saved registers
	for reg in sortedDictValues(registerUseCount):
		if reg in usedRegisters and not reg in mips.callerSavedRegisters:
			mapping[reg] = count
			count = count + 1
			usedRegisters.discard(reg)

	# Left with the caller-saved registers. We will now go through the
	# functions and assign the callersaved registers to different java
	# local variables (to allow local calling)
	for fn in functions:
		cur = mapping.copy()
		left = Set(fn.usedRegisters).intersection(mips.callerSavedRegisters)
		for reg in sortedDictValues(registerUseCount):
			if reg in left:
				cur[reg] = count
				count = count + 1
		out[fn.address] = cur
	return out, count+1


class RegisterHandler:
	def __init__(self, controller, byteCodeGenerator):
		self.controller = controller
		self.byteCodeGenerator = byteCodeGenerator
		self.memoryRegisterOffsets = {}

	def resetMemoryRegisterAllocator(self):
		"Reset the memory register allocator"
		self.memoryRegisters = [ x for x in mips.memoryAddressRegisters ]

	def resetMemoryRegister(self, reg):
		"Reset a memory register"
		self.memoryRegisterOffsets[reg] = 0

	def allocateMemoryRegister(self):
		"Allocate a new memory address register"
		if len(self.memoryRegisters) > 0:
			return self.memoryRegisters.pop(0)
		return None

	def memoryRegisterComputeOffset(self, reg, imm):
		"Compute a new offset to increase/decrease the memory register with"
		out = imm - self.memoryRegisterOffsets[reg]
		self.memoryRegisterOffsets[reg] = imm
		return out

	def deallocateMemoryRegister(self, reg):
		"Deallocate a memory register"
		self.memoryRegisters.insert(0, reg)

	def regToLocalVariable(self, reg):
		"Convert a register value into a local variable value"
		return reg2local[reg]

	# Register-related functions
	def pushRegister(self, reg):
		if reg == 0:
			self.byteCodeGenerator.pushConst(0)
		elif reg == mips.R_MEM:
                        nr = self.regToLocalVariable(reg)
			self.byteCodeGenerator.aload(nr)
		elif regIsStatic(reg):
			self.byteCodeGenerator.getstatic(staticRegs[reg])
		else:
			nr = self.regToLocalVariable(reg)
			self.byteCodeGenerator.iload(nr)

	def popToRegister(self, reg):
		if reg == 0:
			print "WARNING: Trying to assign to register zero. Ignored.", self
			return
		elif reg == mips.R_MEM:
			nr = self.regToLocalVariable(reg)
			self.byteCodeGenerator.astore(nr)
		elif regIsStatic(reg):
			self.byteCodeGenerator.putstatic(staticRegs[reg])
		else:
			nr = self.regToLocalVariable(reg)
			self.byteCodeGenerator.istore(nr)

	def popToRegisterFloat(self, reg):
		if reg == 0 or regIsStatic(reg):
			print "WARNING: Trying to assign a float to register zero or to a static. Ignored.", self
			return
		nr = self.regToLocalVariable(reg)
		self.byteCodeGenerator.fstore(nr)
