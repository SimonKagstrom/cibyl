######################################################################
##
## Copyright (C) 2007,  Simon Kagstrom
##
## Filename:      javaclass.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Java class
##
## $Id: $
##
######################################################################
from Cibyl.BinaryTranslation import bytecode, register
from Cibyl import config


class JavaClass(bytecode.ByteCodeGenerator):
    def __init__(self, controller, name):
        bytecode.ByteCodeGenerator.__init__(controller)
        self.name = name
        self.methods = []

    def getName(self):
        return self.name

    def addMethod(self, method):
        self.methods.append(method)
        method.setJavaClass(self)

    def fixup(self):
        for method in self.methods:
            method.fixup()

    def compile(self):
       	self.emit(".class public %s" % (self.name) )
	self.emit(".super java/lang/Object")
        # Initializer
        self.emit(".method public <init>()V")
        self.emit("aload_0")
        self.emit("invokenonvirtual java/lang/Object.<init>()V")
        self.emit("return")
        self.emit(".end method")

        for method in self.method:
            method.compile()
