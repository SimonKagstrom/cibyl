######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      optimizer.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Optimization infrastructure
##
## $Id: optimizer.py 12822 2006-12-22 15:09:01Z ska $
##
######################################################################
import mips

class Value:
    def __init__(self):
	pass

    def doOperation(self, s, *args):
	if s == "+": return self.__add__(*args)
	if s == "-": return self.__sub__(*args)
	if s == "/": return self.__div__(*args)
	if s == "&": return self.__and__(*args)
	if s == "*": return self.__mul__(*args)
	if s == "^": return self.__xor__(*args)
	if s == "|": return self.__or__(*args)
	if s == ">>": return self.__rshift__(*args)
	if s == "<<": return self.__lshift__(*args)

	try:
	    if s == "==": return KnownValue(self.value == args[0].value)
	    if s == "!=": return KnownValue(self.value != args[0].value)
	    if s == "<": return KnownValue(self.value < args[0].value)
	    if s == "<=": return KnownValue(self.value <= args[0].value)
	    if s == ">": return KnownValue(self.value >= args[0].value)
	    if s == ">=": return KnownValue(self.value >= args[0].value)
	except:
	    return UnknownValue()

	if s == "neg": return self.__neg__(*args)
	if s == "cmpl": return self.__invert__(*args)
	return UnknownValue()

class UnknownValue(Value):
    def __add__(self, other):
	return UnknownValue()

    def __and__(self, other):
	return UnknownValue()

    def __sub__(self, other):
	return UnknownValue()

    def __div__(self, other):
	return UnknownValue()

    def __mul__(self, other):
	return UnknownValue()

    def __xor__(self, other):
	return UnknownValue()

    def __or__(self, other):
	return UnknownValue()

    def __rshift__(self, other):
	return UnknownValue()

    def __lshift__(self, other):
	return UnknownValue()

    def __cmp__(self, other):
	return UnknownValue()

    def __neg__(self):
	return UnknownValue()

    def __invert__(self):
	return UnknownValue()


class KnownValue(Value):
    def __init__(self, value):
	if type(value) == int:
	    value = long(value)
	if value == False:
	    self.value = 0
	if value == True:
	    self.value = 1
	else:
	    self.value = value

    def __add__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value + other.value)

    def __and__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value & other.value)

    def __sub__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value - other.value)

    def __div__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value / other.value)

    def __mul__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value * other.value)

    def __xor__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value ^ other.value)

    def __or__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value | other.value)

    def __rshift__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value >> other.value)

    def __lshift__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value << other.value)

    def __cmp__(self, other):
	if isinstance(other, UnknownValue):
	    return UnknownValue()
    	return KnownValue(self.value - other.value)

    def __neg__(self):
    	return KnownValue(-self.value)

    def __invert__(self):
    	return KnownValue(~self.value)

    def __str__(self):
	return str(self.value)

class Optimizer:
    "This class implements the value-tracking optimizing framework for Cibyl"
    def __init__(self):
	self.registers = [UnknownValue()] * mips.N_REGS
	self.invalidateAllRegisters()

    def invalidateAllRegisters(self, flushFunction = None):
	self.registers[0] = KnownValue(0)
	for i in range(1, mips.N_REGS):
	    if flushFunction and isinstance(self.registers[i], KnownValue):
		flushFunction(i, self.registers[i])
	    self.invalidateRegister(i)

    def invalidateRegister(self, reg):
	if reg == 0:
	    return
	self.registers[reg] = UnknownValue()

    def getRegisterValue(self, reg):
	if reg == 0:
	    return KnownValue(0)
	return self.registers[reg]

    def setRegisterValue(self, reg, val):
	if reg == 0:
	    return # Cannot be written to
	if not isinstance(val, Value):
	    raise Exception("Trying to set register to non-Value object: %s" % val)
	self.registers[reg] = val


if __name__ == "__main__":
    print "5 + -2:", KnownValue(5) + KnownValue(-2)
    print "(-)3:", -KnownValue(3)
    print "8 * 25:", KnownValue(8) * KnownValue(25)
    print "~3:", ~KnownValue(3)
    print "16 >> 2:", KnownValue(16) >> KnownValue(2)
    print "(-)UNKNOWN:", -UnknownValue()

    print "doOp:"
    a = KnownValue(9)
    b = KnownValue(7)
    c = UnknownValue()
    d = KnownValue(9)

    print a, ">", b, ": ", a.doOperation(">", b)
    print a, "<", b, ": ", a.doOperation("<", b)
    print a, "==", c, ": ", a.doOperation("==", c)
    print "-", a, ":", a.doOperation("neg")
    print a, "*", c, ": ", a.doOperation("*", c)
    print a, "<=", d, ": ", a.doOperation("<=", d)
    print a, ">>", b, ": ", a.doOperation(">>", KnownValue(2))

    print "Test the Optimizer class"

    opt = Optimizer()
    opt.setRegisterValue(mips.R_RA, KnownValue(29))
    opt.setRegisterValue(mips.R_SP, KnownValue(100))
    print opt.getRegisterValue(mips.R_RA) + opt.getRegisterValue(mips.R_SP)
    print opt.getRegisterValue(mips.R_RA) + opt.getRegisterValue(mips.R_A0)
