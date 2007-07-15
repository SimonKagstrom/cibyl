######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      mult.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   MIPS multiplication optimization
##
## $Id: mult.py 13919 2007-03-02 07:51:04Z ska $
##
######################################################################
import Cibyl.BinaryTranslation.Mips.mips as mips
import Cibyl.config as config

def run(function):
    """mult/div optimization: If mfhi is not used, do a normal
	Java mul/div and only assign to LO (not for unsigned
	muls/divs)"""
    computeLow = True
    mfhi = 0
    mflo = 0
    div = 0
    mult = 0
    fixups = []

    # Count mfhi/mflo and mult/div instructions
    for insn in function.instructions:
	if insn.opCode == mips.OP_MULT or insn.delayed and insn.delayed.opCode == mips.OP_MULT:
	    mult = mult + 1
	if insn.opCode == mips.OP_DIV or insn.delayed and insn.delayed.opCode == mips.OP_DIV:
	    div = div + 1
	if insn.opCode == mips.OP_MFHI or insn.delayed and insn.delayed.opCode == mips.OP_MFHI:
	    mfhi = mfhi + 1
	if insn.opCode == mips.OP_MFLO or insn.delayed and insn.delayed.opCode == mips.OP_MFLO:
	    mflo = mflo + 1

    # Cannot optimize cases where there are more both mfhi and mflo's
    # or if there is a mult and an mfhi
    if (mult > 0 and mfhi > 0) or (mfhi > 0 and mflo > 0):
	return

    # If division and only mfhi - only compute high result (%),
    # otherwise compute low result
    if div > 0 and mult == 0 and mflo == 0 and mfhi > 0:
	computeLow = False

    # Go through all instructions to fixup
    for insn in function.instructions:
	if insn.opCode in (mips.OP_MULT, mips.OP_DIV):
	    fixups.append(insn)
	if insn.delayed and insn.delayed.opCode in (mips.OP_MULT, mips.OP_DIV):
	    fixups.append(insn.delayed)

    # Fixup all the multiplications and divisions to produce
    # 32-bit results only.
    for insn in fixups:
	if config.verbose: print "MulOpt: replacing", insn, "with a direct Java bytecode"
	if computeLow:
	    insn.compile = insn.compileLowWordResult
	else:
	    insn.compile = insn.compileHighWordResult
