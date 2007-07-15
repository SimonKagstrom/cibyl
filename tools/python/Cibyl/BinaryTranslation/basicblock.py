######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
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

    def registerWrittenToBefore(self, instruction, reg):
        insnsToCheck = self.instructions[ : self.instructions.index(instruction) ]
        for insn in insnsToCheck:
            if reg in insn.getDestinationRegisters():
                return True
        return False

