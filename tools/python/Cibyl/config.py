######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:      config.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Configuration
##
## $Id: config.py 13856 2007-02-24 08:48:25Z ska $
##
######################################################################
import os,sys

verbose = False
outDirectory = "."
pruneUnusedFunctions = True
doConstantPropagation = False
doMultOptimization = False
doMemoryRegisterOptimization = False
doRegisterScheduling = False
doOptimizeIndirectCalls = False

doInlineAllBuiltins = False
inlineBuiltinsFunctions = []

colocateFunctions = []

operandStackLimit = None
classSizeLimit = 12000
callTableHierarchy = 1

debug = False
tracing = False
traceFunctions = None
traceFunctionCalls = False

infile = None
profileFile = None

def getBasePath():
    base = os.getenv("CIBYL_BASE")
    if base == None:
	base = "/usr/local/share/cibyl"
    return base

# setup some of the environment
jasmin = os.getenv("CIBYL_JASMIN", "jasmin")
javac = os.getenv("CIBYL_JAVAC", "javac")
javac_opts = os.getenv("CIBYL_JAVA_OPTS", "")
readelf = os.getenv("CIBYL_READELF", "readelf")
nm = os.getenv("CIBYL_NM", "nm")
objcopy = os.getenv("CIBYL_OBJCOPY", "objcopy")
cpp = os.getenv("CIBYL_CPP", "cpp")

def checkOne(cmdline, fn, error_message):
    f = os.popen(cmdline)
    out = f.read()
    f.close()
    if fn(out) != True:
        print "Failed environment check", cmdline
        print
        print error_message
        print
        return 1
    return 0

def abortWithMessage(msg):
    print
    print "Error:", msg
    sys.exit(1)

def checkEnvironment():
    nok = checkOne("%s -version" % (jasmin),
                  lambda x : x.lower().startswith("jasmin version"),
                  """  Please install a compatible version of jasmin and place it in
  PATH or setup CIBYL_JASMIN in env.sh""")
    # Probably nm and cpp is OK if objcopy is fine
    nok |= checkOne("%s --help" % (objcopy),
             lambda x : "elf32-tradbigmips" in x,
             """  Please install a MIPS binutils or binutils-multiarch (recommended if
  possible) and place it in PATH or setup CIBYL_NM, CIBYL_READELF,
  CIBYL_OBJCOPY and CIBYL_CPP in env.sh""")

    if not os.path.isdir(outDirectory):
        print "The destination " + outDirectory + " is not a directory"
        nok |= 1

    if not os.path.isfile(infile):
        print "The infile '" + infile + "' is not found (or is not a regular file)"
        nok |= 1

    if profileFile and not os.path.isfile(profileFile):
        print "The profile file '" + profileFile + "' is not found (or is not a regular file)"
        nok |= 1

    if nok:
        abortWithMessage("Environment or options incorrect")

if __name__ == "__main__":
    checkEnvironment()
