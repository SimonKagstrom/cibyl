######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      types.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Classes
##
## $Id: classes.py 14172 2007-03-11 14:52:20Z ska $
##
######################################################################
import emit

class PushInstruction:
    pass

class PopInstruction:
    def __init__(self, n=1):
	self.N = n

class Base:
    def __init__(self, line):
	self.line = line
	self.isNop = True
	self.targetLocal = None
	self.sourceLocal = None

    def emit(self):
	emit.out.write(self.line)

    def match(self, what):
	return what.match(self.line) != None

    def __str__(self):
	return self.line

class Nop(Base):
    def __init__(self):
	Base.__init__(self, "")

class Comment(Base):
    pass

class Label(Base):
    def __init__(self, line, name):
	Base.__init__(self, line)
	self.name = name
	self.isNop = False

class Instruction(Base):
    def __init__(self, line):
	Base.__init__(self, line)
	self.isNop = False

class Ireturn(Instruction, PopInstruction):
    pass

class Dup(Instruction, PopInstruction):
    def __init__(self):
	Instruction.__init__(self, "\tdup\n")

class Pop(Instruction, PopInstruction):
    def __init__(self):
	Instruction.__init__(self, "\tpop\n")

class Pop2(Instruction, PopInstruction):
    def __init__(self):
	Instruction.__init__(self, "\tpop2\n")

# iconst, ldc, etc
class Const(Instruction, PushInstruction):
    def __init__(self, value, line=None):
	self.value = long(value)
	if line == None:
	    value = long(value)
	    if value == -1:
		line = "iconst_m1"
	    elif value >= 0 and value <= 5:
		line = "iconst_%d" % (value)
	    elif value >= -128 and value <= 127:
		line = "bipush %d" % (value)
	    elif value >= -32768 and value <= 32767:
		line = "sipush %d" % (value)
	    else:
		line = "ldc %d" % (value)
	    line = "\t" + line + "\n"

	Instruction.__init__(self, line)

class Iinc(Instruction, PushInstruction):
    def __init__(self, line, variable, value):
	Instruction.__init__(self, line)
	self.value = long(value)
	self.targetLocal = variable
	self.sourceLocal = variable

class Istore(Instruction, PopInstruction):
    def __init__(self, line, variable):
	Instruction.__init__(self, line)
	self.targetLocal = variable

class Iload(Instruction, PushInstruction):
    def __init__(self, line, variable):
	Instruction.__init__(self, line)
	self.sourceLocal = variable

class Getstatic(Instruction, PushInstruction):
    def __init__(self, line, variable):
	Instruction.__init__(self, line)
	self.sourceLocal = variable

class Putstatic(Instruction, PopInstruction):
    def __init__(self, line, variable):
	Instruction.__init__(self, line)
	self.targetLocal = variable

class ArithmeticInstruction(Instruction, PushInstruction):
    pass

class BranchInstruction(Instruction, PopInstruction):
    def __init__(self, line, target):
	Instruction.__init__(self, line)
	self.target = target

class ConditionalBranchOne(BranchInstruction, PopInstruction):
    pass

# pops two items from the stack
class ConditionalBranchTwo(BranchInstruction, PopInstruction):
    def __init__(self, line, target):
	PopInstruction.__init__(self, 2)
	BranchInstruction.__init__(self, line, target)

class Invokestatic(BranchInstruction, PopInstruction):
    pass

class Goto(BranchInstruction):
    pass

class Function(Base):
    def __init__(self, line, name, contents):
	Base.__init__(self, line)
	self.name = name
	self.contents = contents
	self.isNop = False

    def emit(self):
	emit.out.write(self.line)
	for item in self.contents:
	    item.emit()
	emit.out.write(".end method")
