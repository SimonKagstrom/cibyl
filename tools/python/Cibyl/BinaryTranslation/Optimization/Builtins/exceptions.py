######################################################################
##
## Copyright (C) 2007,  Simon Kagstrom
##
## Filename:      exceptions.py
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Exception handling (done through builtins)
##
## $Id:$
##
######################################################################
from Cibyl.BinaryTranslation import bytecode, register
from Cibyl.BinaryTranslation.Mips import mips
import builtins
from base import BuiltinBase
from sets import Set

tryInstruction = None

class ExceptionBuiltinBase(BuiltinBase):
    def __init__(self, controller, instruction, name, operation):
        BuiltinBase.__init__(self, controller, instruction, name, operation)
        self.instruction.destinations = Set([mips.R_EAR, mips.R_ECB])
        self.controller.addLabel(self.instruction.address)

class Try(ExceptionBuiltinBase):
    def compile(self):
        global tryInstruction
        self.rh.pushRegister(mips.R_A0)
        self.rh.popToRegister(mips.R_ECB)
        self.rh.pushRegister(mips.R_A1)
        self.rh.popToRegister(mips.R_EAR)
        tryInstruction = self

class Catch(ExceptionBuiltinBase):
    def compile(self):
        javaMethod = self.instruction.getJavaMethod()
        start = tryInstruction.instruction.address
        end = self.instruction.address

        # Get the labels for this exception
        handlerLabel = javaMethod.addExceptionHandler(start, end)
        startLabel = self.controller.getLabel(start)
        endLabel = self.controller.getLabel(end)

        self.bc.emit(".catch all from %s to %s using %s" % (startLabel, endLabel, handlerLabel) )

    def maxOperandStackHeight(self):
        # Conservative estimate: Itself requires a height of 6, but it
        # may be thrown by code which has up to 11 (also pessimistic)
        # entries on the stack
        return 18

def match(controller, instruction, name):
    names = {
	"__NOPH_try": Try,
	"__NOPH_catch": Catch,
	}
    try:
	cur = names[name]
	return cur(controller, instruction, name, None)
    except:
        return None

builtins.alwaysInline.append("__NOPH_try")
builtins.alwaysInline.append("__NOPH_catch")
builtins.addBuiltin(match)
