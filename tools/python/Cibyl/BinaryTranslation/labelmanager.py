######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:      labelmanager.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Label manager class
##
## $Id: labelmanager.py 10981 2006-09-11 20:12:56Z ska $
##
######################################################################
class Label:
    """Container class for labels"""
    def __init__(self, address, inJumpTab = False, name = None, isFunction = False):
	self.label = address
	self.address = address
	self.inJumpTab = inJumpTab
	self.javaMethod = None
	self.isFunction = isFunction
	self.name = None

    def setJavaMethod(self, method):
	"Define which java method this label is in"
	self.javaMethod = method

    def getJavaMethod(self):
	"Return which java method this label is in"
	return self.javaMethod

    def setJavaAddress(self, address):
	"Setup the Java address to emit"
	self.javaAddress = address

    def __cmp__(self, other):
	return cmp(self.address, other.address)

    def __str__(self):
	return ("L_%x" % (self.address))
