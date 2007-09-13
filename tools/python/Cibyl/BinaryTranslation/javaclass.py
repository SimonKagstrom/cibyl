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
import os, subprocess

from Cibyl.BinaryTranslation import bytecode, register
from Cibyl import config


class JavaClass(bytecode.ByteCodeGenerator):
    def __init__(self, controller, name):
        bytecode.ByteCodeGenerator.__init__(self, controller)
        self.name = name
        self.methods = []

    def getName(self):
        return self.name

    def addMethod(self, method):
        self.methods.append(method)
        method.setJavaClass(self)

    def lookupJavaMethod(self, address):
	"Return the java method for a given address"
	for method in self.methods:
            if method == self.controller.getGlobalCallTableMethod():
                continue
            for fn in method.functions:
                if address in (fn.address, fn.address + fn.size - 1):
                    return method
	return None

    def lookupFunction(self, address):
        "Return the function for a given address"
	for method in self.methods:
            for fn in method.functions:
                if address in (fn.address, fn.address + fn.size - 1):
                    return fn
        return None

    def fixup(self):
        for method in self.methods:
            method.fixup()

    def forkProcess(self, data):
        class JasminProcess:
            def __init__(self, name, data):
                self.filename = config.outDirectory + "/" + name + ".j"
                fd = open(self.filename, "w")
                # Write the data to the tempfile
                fd.write(data)
                fd.close()

                # Fork jasmin
                self.process = subprocess.Popen([ config.jasmin,
                                                  "-d", config.outDirectory,
                                                  self.filename ])
        return JasminProcess(self.name, data)


    def compile(self):
	if config.debug:
	    self.emit(".source %s.j" % (self.name))

       	self.controller.emit(".class public %s" % (self.name) )
	self.controller.emit(".super java/lang/Object")

        # Initializer
        self.controller.emit(".method public <init>()V")
        self.controller.emit("aload_0")
        self.controller.emit("invokenonvirtual java/lang/Object.<init>()V")
        self.controller.emit("return")
        self.controller.emit(".end method")

        for method in self.methods:
            method.compile()
            # Add some newlines after the method
            for i in range(0,3):
                self.emit('')

class JavaClassHighLevel(JavaClass):
    def forkProcess(self, data):
        class JavaProcess:
            def __init__(self, controller, name, data):
                self.controller = controller
                self.filename = config.outDirectory + "/" + name + ".java"
                fd = open(self.filename, "w")
                # Write the data to the tempfile
                fd.write(data)
                fd.close()

                # Wait for all the Jasmin processes to finish
                for p in self.controller.processes:
                    "Wait for the process to complete and clean up after it"
                    try:
                        os.waitpid(p.process.pid, 0)
                    except:
                        pass

                # Fork java
                self.process = subprocess.Popen([ config.javac, "-d", config.outDirectory,
                                                  "-classpath", config.outDirectory,
                                                  self.filename ] + config.javac_opts.split() )
        return JavaProcess(self.controller, self.name, data)

    def compile(self):
        self.controller.emit("class %s {" % self.name)

        for method in self.methods:
            method.compile()
        self.emit("}")

