######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      function.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Function defs (inherits codeblock)
##
## $Id: function.py 13391 2007-02-02 12:26:28Z ska $
##
######################################################################
from sets import Set
from Cibyl.BinaryTranslation.codeblock import CodeBlock
from Cibyl.BinaryTranslation.basicblock import BasicBlock

class Function(CodeBlock):
    """Container class for functions"""
    def __init__(self, controller, name, instructions, labels, trace=False):
	CodeBlock.__init__(self, controller, instructions, labels, trace)
	self.name = name
	self.basicBlocks = []

	# Assume this is a leaf function
	self.isLeafFunction = True

	# Create basic blocks and check leafness
	tmp = []
	for insn in self.instructions:
	    tmp.append(insn)
	    if insn.isBranch or insn.isBranchDestination:
		# Remove the last instruction if this is a branch -
		# but not for the function entry point. The reason is
		# that some functions might start with a branch
		if insn.isBranchDestination and not insn.address == self.address:
		    tmp = tmp[:-1]
		if tmp != []:
		    # Do not append empty basic blocks
		    bb = BasicBlock(self.controller, tmp, self.labels, trace)
		    self.basicBlocks.append(bb)
		tmp = []
		if insn.isBranchDestination and not insn.address == self.address:
		    tmp.append(insn)

	    if insn.isFunctionCall:
		self.isLeafFunction = False
            insn.setFunction(self)
	else:
	    # Last one
	    if tmp != []:
		self.basicBlocks.append(BasicBlock(self.controller, tmp, self.labels, trace))


    def isLeaf(self):
	return self.isLeafFunction

    def getReturnBasicBlocks(self):
	out = []
	for bb in self.basicBlocks:
	    if bb.isReturnBlock:
		out.append(bb)
	return out


    def compile(self):
	"Compile this basic block to Java assembly"
	for bb in self.basicBlocks:
	    bb.compile()

    def __str__(self):
	out = "%s(%d):\n" % (self.name, self.getByteCodeSize())
	return out + CodeBlock.__str__(self)
