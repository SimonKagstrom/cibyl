######################################################################
##
## Copyright (C) 2007,  Simon Kagstrom
##
## Filename:	  base.py
## Author:		Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Base-class for the builtins
##
## $Id:$
##
######################################################################
from Cibyl.BinaryTranslation import bytecode, register

class BuiltinBase:
	def __init__(self, controller, instruction, name, operation):
		self.name = name
		self.operation = operation
		self.instruction = instruction
		self.controller = controller
		self.bc = bytecode.ByteCodeGenerator(controller)
		self.rh = register.RegisterHandler(controller, self.bc)

	def maxOperandStackHeight(self):
		return 2

	def compile(self):
		pass
