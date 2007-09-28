######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:      basicblock.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Basic block stuff (inherits from CodeBlock)
##
## $Id: basicblock.py 13391 2007-02-02 12:26:28Z ska $
##
######################################################################
from Cibyl.BinaryTranslation.codeblock import CodeBlock

class BasicBlock(CodeBlock):
    def __init__(self, controller, instructions, labels, trace):
	CodeBlock.__init__(self, controller, instructions, labels, trace)

        for insn in self.instructions:
            insn.setBasicBlock(self)
	# If the last instruction is a return, then this is a return
	# block
	self.isReturnBlock = instructions[-1].isReturnInstruction

    def registerReadBeforeWritten(self, reg):
        for insn in self.instructions:
            if insn.isDisabled():
                continue
            if reg in insn.getSourceRegisters():
                return True
            if reg in insn.getDestinationRegisters():
                return False
        # Assume true for unused registers
        return True

    def raUsed(self, reg):
        for insn in self.instructions:
            if insn.isDisabled():
                continue
            if reg in insn.getSourceRegisters() and not insn.isFunctionCall:
                return True
            if reg in insn.getDestinationRegisters() and not insn.isFunctionCall:
                return True
        return False
