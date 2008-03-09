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
import sys, re, os, tempfile, struct
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
		outfile.write(") %s\n" % (item.getQualifier()) )
	    outfile.close()


class SyscallWrapperGenerator(SyscallGenerator):
    def __init__(self, program, syscallDirectories, dirs, syscallSets, outdir, defines=[], generateAllSyscalls = False):
	self.dirs = dirs
	self.syscallSets = generateSyscallSetDependencies(self.dirs, syscallSets)
	self.functions = functionsFromHeaderDirectories(syscallDirectories)

	self.generateAllSyscalls = generateAllSyscalls
	if generateAllSyscalls:
	    self.controller = None
	else:
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
		if (self.generateAllSyscalls or self.controller.usesSyscall(item.name)) and fileExists(curDir + "/" + item.getSyscallSet()):
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



class SyscallDatabaseGenerator(SyscallGenerator):
    def __init__(self, dirname, syscallSets, outfile):
	SyscallGenerator.__init__(self, [dirname], syscallSets)
	self.outfile = outfile

    def encodeReturnType(self, item):
        if item.getJavaReturnType() == "void":
            return 0
        if item.getJavaReturnType() == "int":
            return 1
        if item.getJavaReturnType() == "boolean":
            return 2

        # objref
        return 3

    def encodeQualifier(self, item):
        # Assume no qualifier
        out = 0
        if item.getQualifier() == "/* Throws */":
            out = out | 1
        if item.getQualifier() == "/* Not generated */":
            out = out | 2
        return out

    def encodeArgumentJavaType(self, arg):
        offs = self.add_str(arg.getJavaType())
        t = 0
        if arg.isObjectReference():
            t = 1

        return ((t << 24) | offs)

    def encodeArgumentType(self, arg):
        offs = self.add_str(arg.getType())
        t = 0

        return ((t << 24) | offs)

    def add_str(self, s):
        # Add the string including null-termination
        if s == None:
            s = ""
        out = self.strtab_offs
        self.strtab_offs = self.strtab_offs + len(s) + 1
        self.strs.append(s)
        return out

    def run(self):
        items = []
        strtab = {}
        self.strtab_offs = 0
        self.strs = []

        # Read all syscall directories
        of = open(self.outfile, "w")
	for curFile in self.files:
	    for item in curFile.all:
		if not isinstance(item, Function):
		    continue
                items.append(item)

        # Create the structure
        out = []
        args = []
        for item in items:
            name_offs = self.add_str(item.getName())
            javaClass_offs = self.add_str(item.getJavaClass())
            javaMethod_offs = self.add_str(item.getJavaMethod())
            out.append( ( item.getNr(), self.encodeReturnType(item), item.getNrArgs(),
                          self.encodeQualifier(item), name_offs, javaClass_offs,
                          javaMethod_offs, self.add_str(item.getSyscallSet()), self.add_str(item.getJavaReturnType()) ) )
            if item.getNrArgs() != 0:
                for arg in item.args:
                    args.append((self.encodeArgumentJavaType(arg),
                                 self.encodeArgumentType(arg),
                                 self.add_str(arg.getName())))

        # Size of each struct is 11
        arg_offs = 6 * 4 + 4 * len(self.dirs) + 4 * len(self.syscallSets) + 11 * len(out) * 4
        strtab_offs = arg_offs + 4 * len(args) * 4

        # Write the header
        of.write(struct.pack(">L", 0xa1b1c1d1)) # magic
        of.write(struct.pack(">L", len(self.dirs))) # Nr syscall directories
        of.write(struct.pack(">L", len(self.syscallSets))) # Nr syscall sets
        of.write(struct.pack(">L", len(out)) )      # Nr items
        of.write(struct.pack(">L", arg_offs))
        of.write(struct.pack(">L", strtab_offs))

        # The path names, as strtab offsets
        for d in self.dirs:
            offs = self.add_str(os.path.abspath(d))
            of.write(struct.pack(">L", offs))

        # The syscall set names, as strtab offsets
        for s in self.syscallSets:
            offs = self.add_str(s)
            of.write(struct.pack(">L", offs))

        # Write the out structures
        arg_count = 0
        for s in out:
            of.write(struct.pack(">LLLLLLLLLLL",
                                 s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8],
                                 arg_count, 0)) # Last is for usage outside
            arg_count = arg_count + 4 * 4 * s[2]

        # Write the arguments
        for a in args:
            of.write(struct.pack(">LLLL", 0, a[0], a[1], a[2]))

        # Write the string table
        for item in self.strs:
            of.write(item + '\0')

        of.close()
