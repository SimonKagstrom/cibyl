######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:	  memoryRegisters.py
## Author:		Simon Kagstrom <ska@bth.se>
## Description:   Memory register optimization for MIPS
##
## $Id: memoryRegisters.py 12898 2007-01-06 10:12:15Z ska $
##
######################################################################
from Cibyl import config
from Cibyl.BinaryTranslation.Mips import mips
from Cibyl.BinaryTranslation.Mips.instruction import AssignMemoryRegister
from sets import Set

def run(basicBlock):
	"Perform the translation of memory registers"
	memops = [x for x in basicBlock.instructions if x.opCode in (mips.OP_LW, mips.OP_SW)]
	regs = Set()

	basicBlock.controller.registerHandler.resetMemoryRegisterAllocator()
	# Create a set of the address registers
	for insn in memops:
		for s in insn.sources:
			regs.add(s)

	# Count the number of source reg occurances in the memops
	for r in regs:
		insns = [x for x in basicBlock.instructions if r in x.sources.union(x.destinations)]
		# Get the affected instructions and count the ops
		mop = False
		affected = []
		count = [0,0]
		for insn in insns:
			if insn.isDisabled():
				continue
	
			if insn.opCode in (mips.OP_LW, mips.OP_SW):
				# Skip stores/loads to RA
				if insn.rt == mips.R_RA or insn.rt == insn.rs:
					continue
				mop = True
			elif mop == False:
				continue
			affected.append(insn)
			if r == insn.rs and insn.opCode in (mips.OP_LW, mips.OP_SW):
				count[0] = count[0] + 1
			elif r in insn.destinations:
				count[1] = count[1] + 1
	
		# Heuristics to chose which memops to replace
		if count[0] > 2 and count[1] < count[0] - 2:
			memreg = basicBlock.controller.registerHandler.allocateMemoryRegister()
			if not memreg:
				break
			if config.verbose: print "Replacing", mips.registerNames[r], "with", mips.registerNames[memreg], "at basic block 0x%x" % (basicBlock.address)
	
			# Replace all memory access source registers with this and insert new
			# assignment instructions after destinations
			invalid = True
			for insn in affected:
				if r == insn.rs and insn.opCode in (mips.OP_LW, mips.OP_SW):
					if invalid:
						insn.prefix = AssignMemoryRegister(basicBlock.controller, insn.address,
									   mips.CIBYL_ASSIGN_MEMREG, 0, memreg, r, 0,0)
						if insn.opCode == mips.OP_LW:
							basicBlock.addDestinationRegister(memreg)
						if insn.opCode == mips.OP_SW:
							basicBlock.addSourceRegister(memreg)
					if config.verbose: print "Use memreg", mips.registerNames[memreg] , "at", insn
					invalid = False
					insn.rs = memreg
				if r in insn.destinations:
					invalid = True
