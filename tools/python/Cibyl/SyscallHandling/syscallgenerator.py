######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      SyscallGenerator.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   System call generators
##
## $Id: syscallgenerator.py 14099 2007-03-10 07:51:59Z ska $
##
######################################################################
import sys, re, os, tempfile
from sets import Set
from Cibyl.BinaryTranslation.translator import Controller
from Cibyl import config
from function import *

def preprocess(defines, lines):
    s = ""
    for item in defines:
	s = s + "%s " % item
    # pipe cpp, -C menas keep comments, -P means emit no line information
    stdin, stdout = os.popen2("%s -C -P %s" % (config.cpp, s))
    if isinstance(lines, str):
	stdin.write(lines)
    else: # List
	stdin.writelines(lines)
    stdin.close()
    out = stdout.readlines()
    stdout.close()
    return out

def readFile(name):
    try:
	f = open(name)
	out = f.readlines()
	f.close()
	return out
    except:
	# No such file
	return []

def readPreprocessedFile(name, defines):
    return preprocess(defines, readFile(name))

def fileExists(name):
    "Ugly hack warning"
    try:
	os.stat(name)
	return True
    except:
	return False


def generateSyscallSetDependencies(dirs, syscallSets):
    unresolved = []
    for syscallDir in syscallSets:
	unresolved.append(syscallDir)

    while unresolved != []:
	for syscallDir in unresolved:
	    # Add dependencies to the sets
	    for curDir in dirs:
		for line in readFile(curDir + "/" + syscallDir + "/depends"):
		    line = line.strip()
		    if line in unresolved:
			continue
		    # Add this dependency
		    unresolved.insert(0, line)
		    syscallSets.insert(0, line)
		if syscallDir in unresolved:
		    unresolved.remove(syscallDir)

    # Remove duplicates
    return Set(syscallSets)


class SyscallFile:
    def __init__(self, filename):
	self.filename = filename
	self.all = []
	self.functions = []

class SyscallGenerator:
    def __init__(self, dirs, syscallSets):
	self.functions = []
	self.files = []
	self.dirs = dirs
	self.syscallSets = generateSyscallSetDependencies(self.dirs, syscallSets)

	count = 0
	for curDir in self.dirs:
	    for syscallDir in self.syscallSets:
		# Read all the functions for this syscall dir
		for root, dirs, files in os.walk(curDir + "/" + syscallDir + "/include"):
		    for cur in files:
			if not cur.endswith(".h"):
			    continue
			pathToFile = os.path.join(root, cur)
			relativePath = pathToFile.replace(curDir + "/" + syscallDir + "/include/", "")
			curFile = SyscallFile(relativePath)
			f = readFile(os.path.join(root, cur))

			for line in f:
			    match = fnRegexp.match(line)
			    if match == None:
				curFile.all.append(line.strip())
				continue
			    returnType = match.group(1)
			    name = match.group(2).strip()
			    arguments = match.group(3).split(",")
			    qualifier = match.group(4)

			    # Create a new function
			    function = Function(count, syscallDir,
						returnType, name, arguments, qualifier)
			    count = count + 1

			    curFile.all.append(function)
			    curFile.functions.append(function)
			    self.functions.append(function)
			self.files.append(curFile)


class SyscallHeaderGenerator(SyscallGenerator):
    def __init__(self, dirname, syscallSets, outdir):
	SyscallGenerator.__init__(self, [dirname], syscallSets)
	self.outdir = outdir

    def run(self):
	for curFile in self.files:
	    path = "%s/%s" % (self.outdir, curFile.filename)
	    try:
		os.makedirs(os.path.dirname(path))
	    except:
		pass # We do nothing - the path already exists
	    outfile = open(path, "w")
	    for item in curFile.all:
		if not isinstance(item, Function):
		    outfile.write("%s\n" % item)
		    continue
		outfile.write( "#define __NR_%s %d /* %s */\n" % (item.getName(), item.getNr(), item.getSyscallSet()) )
		outfile.write( "static inline _syscall%d(%s,%s" % (item.getNrArgs(), item.getReturnType(), item.getName()) )
		if item.getNrArgs() > 0:
		    for arg in item.getArgs():
			outfile.write(", %s" % arg)
		outfile.write("); %s\n" % (item.getQualifier()) )
	    outfile.close()


class SyscallWrapperGenerator(SyscallGenerator):
    def __init__(self, program, syscallDirectories, dirs, syscallSets, outdir, defines=[]):
	self.dirs = dirs
	self.syscallSets = generateSyscallSetDependencies(self.dirs, syscallSets)
	self.functions = functionsFromHeaderDirectories(syscallDirectories)

	self.controller = Controller(program, syscallDirectories, onlyReadSyscalls=True)
	self.outdir = outdir
	self.outfile = open(outdir + "/Syscalls.java", "w")
	self.defines = defines

    def run(self):
	"""
	Generate Java systemcall wrappers
	"""
	self.outfile.write("/* GENERATED, DON'T EDIT! */\n")

	for curDir in self.dirs:
	    for syscallDir in self.syscallSets:
		# Add all the imports
		for line in readPreprocessedFile(curDir + "/" + syscallDir + "/imports", self.defines):
		    self.outfile.write("%s\n" % line.strip())

	self.outfile.write("public class Syscalls {\n")

	for curDir in self.dirs:
	    for syscallDir in self.syscallSets:
		# And run the initialization
		for line in readPreprocessedFile(curDir + "/" + syscallDir + "/init", self.defines):
		    self.outfile.write("%s\n" % line.strip())

	for curDir in self.dirs:
	    lines = []
	    for item in self.functions:
		if self.controller.usesSyscall(item.name) and fileExists(curDir + "/" + item.getSyscallSet()):
		    lines.append(item.generateJavaCall(curDir + "/" + item.getSyscallSet() ))
	    self.outfile.writelines( preprocess(self.defines, lines) )
	self.outfile.write("}\n")

	for s in self.syscallSets:
	    for curDir in self.dirs:
		if not fileExists(curDir + "/" + s + "/classes"):
		    continue
		for f in os.listdir(curDir + "/" + s + "/classes"):
		    if f.endswith(".java"):
			data = readPreprocessedFile(curDir + "/" + s + "/classes/" + f, self.defines)
			out = open("%s/%s" % (self.outdir, f), "w")
			out.writelines(data)
			out.close()
