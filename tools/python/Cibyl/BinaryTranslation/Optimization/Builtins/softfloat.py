######################################################################
##
## Copyright (C) 2007,  Simon Kagstrom
##
## Filename:      softfloat.py
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Soft-float "inlined" functions
##
## $Id:$
##
######################################################################
from Cibyl.BinaryTranslation import bytecode, register
from Cibyl.BinaryTranslation.Mips import mips
import builtins
from base import BuiltinBase

class Arithmetic1(BuiltinBase):
    def compile(self):
        # Convert the source register
        self.rh.pushRegister(mips.R_A0)
        self.bc.invokestatic("java/lang/Float/intBitsToFloat(I)F")

        # Perform the actual operation
        self.bc.emit(self.operation)

        # Pop the result to v0
        self.bc.invokestatic("java/lang/Float/floatToIntBits(F)I")
        self.rh.popToRegister(mips.R_V0)

class Arithmetic2(BuiltinBase):
    def compile(self):
        # Convert the source registers
        self.rh.pushRegister(mips.R_A0)
        self.bc.invokestatic("java/lang/Float/intBitsToFloat(I)F")
        self.rh.pushRegister(mips.R_A1)
        self.bc.invokestatic("java/lang/Float/intBitsToFloat(I)F")

        # Perform the actual operation
        self.bc.emit(self.operation)

        # Pop the result to v0
        self.bc.invokestatic("java/lang/Float/floatToIntBits(F)I")
        self.rh.popToRegister(mips.R_V0)

class Compare(BuiltinBase):
    def compile(self):
        # Convert the source registers
        self.rh.pushRegister(mips.R_A0)
        self.bc.invokestatic("java/lang/Float/intBitsToFloat(I)F")
        self.rh.pushRegister(mips.R_A1)
        self.bc.invokestatic("java/lang/Float/intBitsToFloat(I)F")

        # Perform the actual operation
        self.bc.emit(self.operation)

        # Pop the result to v0
        self.rh.popToRegister(mips.R_V0)

class IntToFloat(BuiltinBase):
    def compile(self):
        # Convert the source registers
        self.rh.pushRegister(mips.R_A0)
        self.bc.emit("i2f")
        self.bc.invokestatic("java/lang/Float/floatToIntBits(F)I")
        self.rh.popToRegister(mips.R_V0)

class FloatToInt(BuiltinBase):
    def compile(self):
        # Convert the source registers
        self.rh.pushRegister(mips.R_A0)
        self.bc.invokestatic("java/lang/Float/intBitsToFloat(I)F")
        self.bc.emit("f2i")
        self.rh.popToRegister(mips.R_V0)

def match(controller, name):
    names = {
        # Arithmetic (1 operand)
        "__negsf2" : [Arithmetic1, "fneg"],
        # Arithmetic (2 operands)
        "__addsf3" : [Arithmetic2, "fadd"],
        "__subsf3" : [Arithmetic2, "fsub"],
        "__divsf3" : [Arithmetic2, "fdiv"],
        "__mulsf3" : [Arithmetic2, "fmul"],
        # Comparisons
        "__eqsf2"  : [Compare, "fcmpg"],
        "__nesf2"  : [Compare, "fcmpg"],
        "__nesf2"  : [Compare, "fcmpg"],
        "__lesf2"  : [Compare, "fcmpg"],
        "__ltsf2"  : [Compare, "fcmpl"],
        # Conversion
        "__floatsisf": [IntToFloat, None],
        "__fixsfsi": [FloatToInt, None],
        "__fixsfdi": [FloatToInt, None],
        }
    try:
        cur = names[name]
        return cur[0](controller, name, cur[1])
    except:
        return None

builtins.alwaysInline.append("__floatsisf")
builtins.alwaysInline.append("__fixsfsi")
builtins.alwaysInline.append("__fixsfdi")
builtins.addBuiltin(match)
