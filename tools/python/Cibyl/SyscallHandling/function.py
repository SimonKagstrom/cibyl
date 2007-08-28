######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      SyscallGenerator.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   System call generators
##
## $Id: function.py 13577 2007-02-10 17:25:13Z ska $
##
######################################################################
import re,os

fnRegexp = re.compile("([A-Z,a-z,0-9,_,\*]+) ([A-Z,a-z,0-9,_]+)\(([A-Z,a-z,0-9,\*,\,, ,_]*)\);[ ]*(\/\*[A-Z,a-z, ]+\*\/)*")
javaTypeRegexp = re.compile("NOPH_([A-Z,a-z,0-9]+)_t")
javaClassMethodRegexp = re.compile("NOPH_([A-Z,a-z,0-9]+)_([A-Z,a-z,0-9]+)_*([A-Z,a-z,0-9]+)*")

verbose = 0

# Hack-warning. This file is implemented in an ugly way. Sorry about
# that.

class Arg:
    def __init__(self, string, nr):
	both = string.split()

	# Handle qualifiers specially
	if both[0] in ["const", "unsigned"]:
	    both[1] = both[0] + " " + both[1]
	    both = both[1:]

	self.type = both[0]
	self.javaType = None
	self.name = ""
	self.objectReference = False
	if self.type != "void":
	    self.name = both[1]
	    self.javaType = self.type # For ints etc
	    match = javaTypeRegexp.match(self.type)
	    if match:
		# This is a Java object reference
		self.objectReference = True
		self.javaType = match.group(1)
	    elif "char*" in self.javaType:
		self.javaType = "String"
	    elif "bool_t" in self.javaType:
		self.javaType = "boolean"
	self.nr = nr
    def isObjectReference(self):
	return self.objectReference
    def getType(self):
	return self.type
    def getJavaType(self):
	return self.javaType
    def getName(self):
	return self.name

    def generateJavaGetRegisterValue(self):
	if self.javaType == None:
	    # Void
	    return ""
	if self.javaType == self.type:
	    return "%s" % (self.getName())
	elif self.javaType == "boolean":
	    return "(boolean) (__%s == 0 ? false : true)" % (self.getName())
	elif self.javaType == "String":
	    return "CRunTime.charPtrToString(__%s)" % (self.getName())
	return "(%s)CRunTime.objectRepository[__%s]" % (self.javaType, self.getName())

    def generateJavaGetArgument(self):
	out = ""
	if self.javaType == None:
	    # Void
	    return
	if verbose:
	    out = 'System.out.println("   " + %s);\n\t\t' % (self.generateJavaGetRegisterValue())
	if self.javaType != self.type:
	    return out + "%s %s = %s;" % (self.javaType, self.name, self.generateJavaGetRegisterValue())

    def __str__(self):
	if self.name == "":
	    return self.type
	return "%s, %s" % (self.type, self.name)

class Function:
    def __init__(self, nr, syscallSet, returnType, name, arguments, qualifier):
	self.name = name
	self.syscallSet = syscallSet #.replace(".", "_")
	self.returnType = returnType
	self.qualifier = qualifier
	self.args = []
	self.nr = nr
	self.argumentStrings = arguments

	nr = 0
	for cur in arguments:
	    self.args.append(Arg(cur, nr))
	    nr = nr + 1
	self.javaClass = None
	self.javaMethod = None
	self.javaReturnType = self.returnType
	match = javaTypeRegexp.match(self.returnType)
	if match:
	    self.javaReturnType = match.group(1)
        elif returnType == "bool_t":
            self.javaReturnType = "boolean"

	match = javaClassMethodRegexp.match(self.name)
	if match:
	    self.javaClass = match.group(1)
	    self.javaMethod = match.group(2)

    def getNr(self):
	return self.nr

    def getNrArgs(self):
	nr = len(self.args)
	if len(self.args) == 1 and self.args[0].getType() == "void":
	    return 0
	return nr

    def getName(self):
	return self.name

    def getReturnType(self):
	return self.returnType

    def getJavaReturnType(self):
	return self.javaReturnType

    def getQualifier(self):
	if not self.qualifier:
	    return ""
	return self.qualifier

    def getArgs(self):
	return self.args

    def getSyscallSet(self):
	return self.syscallSet

    def getJavaClass(self):
	return self.javaClass

    def getJavaMethod(self):
	return self.javaMethod

    def generateJavaCall(self, dirname):
	"""
	Genereate the java wrapper for this function. This function is
	somewhat complex still, but it's not as bad as it looks :-)
	"""
	args = self.args

	if self.getQualifier() == "/* Not generated */":
	    template = open("%s/implementation/%s.java" % (dirname, self.getName()))
	    return template.read()
	elif self.getJavaClass() == None or self.getJavaMethod == None:
	    return ""
	if self.getReturnType() == "void":
	    ret = "void"
	else:
	    ret = "int"
	out = "\tpublic static final %s %s(" % (ret, self.getName())
	for i in range(0, self.getNrArgs()):
	    addend = ""
	    if i < self.getNrArgs() - 1:
		addend = ", "
	    arg = self.args[i]
	    if arg.getType() == arg.getJavaType():
		out += "int %s%s" % (arg.getName(), addend)
	    else:
		out += "int __%s%s" % (arg.getName(), addend)

        throws = ""
	if self.getQualifier() == "/* Throws */":
            throws = "throws Exception"
	out += ") %s {" % (throws)
	if verbose:
	    out = out + '\n\tSystem.out.println("%s");\n' % (self.getName())

	for arg in args:
	    argString = arg.generateJavaGetArgument()
	    if argString != None:
		out = out + "\n\t\t" + argString
	out += "\n\n\t\t"

	if self.getReturnType() != "void":
	    out += "%s ret = (%s)" % (self.getJavaReturnType(), self.getJavaReturnType())

	# Generate the actual call, constructors are handled separately
	if self.getJavaMethod() == "new":
	    out = out + "new %s(" % ( self.getJavaClass())
	else:
	    if (args != [] and args[0].isObjectReference()) and args[0].getJavaType() == self.javaClass:
		# Object call
		out = out + "%s.%s(" % ( args[0].getName(), self.getJavaMethod() )
		# Remove the first argument (the "this" pointer)
		args = args[1:]
	    else:
		# Static call
		out = out + "%s.%s(" % ( self.getJavaClass(), self.getJavaMethod() )

	for arg in args:
	    out += "%s" % (arg.getName())
	    if arg != args[-1]:
		out += ", "
	out = out + ");\n"

	# Handle the return value
	if self.getReturnType() != "void":
	    retName = "ret"
	    if self.getJavaReturnType() != self.getReturnType() and self.getJavaReturnType() != "boolean":
		out = out + "\t\tint registeredHandle = CRunTime.registerObject(%s);\n" % (retName)
		retName = "registeredHandle"
	    if verbose:
		out = out + '\n\tSystem.out.println("   Return: " + %s);\n' % (retName)
            if self.getJavaReturnType() == "boolean":
                out = out + "\t\treturn %s ? 1 : 0;\n" % (retName)
            else:
                out = out + "\t\treturn %s;\n" % (retName)

	return out + "\t}\n"

    def __str__(self):
	argStrings = ""
	for arg in self.argumentStrings:
	    argStrings += "%s," % (arg)

	argStrings = argStrings[:-1]
	out = "%d %s %s(%s); %s" % (self.nr, self.returnType, self.name, argStrings, self.qualifier)
	return out



# Regexps are nice for things like this :-)
syscallDefinitionRegexp = re.compile("[ \t]*static inline _syscall([0-9]+)\(([A-Z,a-z,0-9,_,\*]+)[ \t\,]*,([A-Z,a-z,0-9,_]+)[ \t]*([A-Z,a-z,0-9,\*,\,, ,_]*)\); (\/\*[ ]+[A-Z,a-z,_,0-9, ]+[ ]+\*\/)*")
nrRegexp = re.compile("#define __NR_([a-z,A-Z,0-9,_]+) ([0-9]+) \/\*[ ]+([A-Z,a-z, ,_,0-9,.,-]+)[ ]+\*\/*")
def functionsFromHeaderDir(dirname):
    "Open the numbers file and read the system call numbers"
    out = []

    lastNr = 0
    for root, dirs, files in os.walk(dirname):
	for cur in files:
	    if not cur.endswith(".h"):
		continue
	    sysc = open(os.path.join(root, cur))

	    for line in sysc:
                s = line.strip()
                if not (s.startswith("static inline") or s.startswith("#define")):
                    continue
		nrMatch = nrRegexp.match(line)
		if nrMatch:
		    lastNr = int(nrMatch.group(2))
		    syscallSet = nrMatch.group(3)
                    continue

		fnMatch = syscallDefinitionRegexp.match(line)
		if fnMatch:
		    nrArgs = int(fnMatch.group(1))
		    retType = fnMatch.group(2)
		    name = fnMatch.group(3).strip(",")
		    args = fnMatch.group(4).split(",")
		    qualifier = fnMatch.group(5)
		    arguments = []
		    #Create argument list
		    for i in range(0, nrArgs):
			arguments.append( args[i*2] + " " + args[i*2+1] )
		    out.append( Function(lastNr, syscallSet,
					 retType, name,
					 arguments, qualifier) )
	    sysc.close()

    return out

def functionsFromHeaderDirectories(dirs):
    out = []
    for curDir in dirs:
	out = out + functionsFromHeaderDir(curDir)
    return out
