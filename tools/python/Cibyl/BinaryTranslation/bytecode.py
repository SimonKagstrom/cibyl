######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      bytecode.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Java bytecode stuff
##
## $Id: bytecode.py 12821 2006-12-22 14:46:47Z ska $
##
######################################################################
opcodeSize = {
    "ishr"   : 1,
    "ishl"   : 1,
    "iushr"  : 1,
    "iadd"   : 1,
    "isub"   : 1,
    "iand"   : 1,
    "iload"  : 1,
    "iload_0": 1,
    "iload_1": 1,
    "iload_2": 1,
    "iload_3": 1,
}

def sizeof(what):
    "Returns the size of the instruction and constant passed"
    return 1

class ByteCodeGenerator:
    def __init__(self, controller):
	self.controller = controller

    def goto(self, address):
	if type(address) == str:
	    self.emit("goto %s" % (address) )
	else:
	    self.emit("goto %s" % ( str(self.controller.getLabel(address))) )

    def pushConst(self, val):
	if val == -1:
	    self.emit("iconst_m1")
	elif val >= 0 and val <= 5:
	    self.emit("iconst_%d" % (val) )
	elif val >= -128 and val <= 127:
	    self.emit("bipush %d" % (val) )
	elif val >= -32768 and val <= 32767:
	    self.emit("sipush %d" % (val) )
	else:
	    self.emit("ldc %d" % (val) )

    def dup(self):
	self.emit('dup')

    def ldc(self, s):
	self.emit('ldc "%s"' % (s))

    def load_store_helper(self, type, nr):
	if nr >= 0 and nr <= 3:
	    self.emit("%s_%d" % (type, nr))
	else:
	    self.emit("%s %d" % (type, nr))

    def iload(self, nr):
        self.load_store_helper("iload", nr)

    def istore(self, nr):
        self.load_store_helper("istore", nr)

    def fload(self, nr):
        self.load_store_helper("fload", nr)

    def fstore(self, nr):
        self.load_store_helper("fstore", nr)

    def getstatic(self, static):
	"Push the value of a static variable on the stack"
	self.emit("getstatic %s" % (static))

    def putstatic(self, static):
	"Update the value of a static variable from the stack"
	self.emit("putstatic %s" % (static))

    def iinc(self, reg, val):
	"Increment reg by 1 (optimize add rt, rt, 1)"
	self.emit('iinc %d %d' % (self.regToLocalVariable(reg), val))

    def pop(self):
	self.emit('pop')

    def ireturn(self):
	self.emit('ireturn')

    def emit_return(self):
	self.emit('return')

    def lookupswitch(self, table, default):
	self.emit("lookupswitch")
	table.sort()
	for vals in table:
	    k, v = vals
	    self.controller.emit( "\t\t0x%x : %s" % (k, v) )
	self.controller.emit("\t\tdefault: %s" % default)

    def tableswitch(self, table, default):
	"Emit the tableswitch mnemonic"
	table.sort()
	kstart, v = table[0]
	klast, v = table[-1]
	self.emit("tableswitch 0x%x 0x%x"  % (kstart, klast) )
	for vals in table:
	    k, v = vals
	    self.emit( "\t%s" % (v) )
	self.emit("\tdefault: %s" % default)

    def invokestatic(self, fn):
	self.emit('invokestatic %s' % (fn) )

    def getSize(self):
	return 1 # At least :-)

    def emit(self, what):
	self.controller.emit("\t%s" % what)
