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

import which

verbose = False
outDirectory = "."
pruneUnusedFunctions = True
doConstantPropagation = False
doMultOptimization = False
doRegisterScheduling = False
doOptimizeIndirectCalls = False
doPeepholeOptimize = False
doRegisterValueTracking = False
doOptimizePruneStackStores = False

aloadMemory = True

doInlineAllBuiltins = False
inlineBuiltinsFunctions = []

colocateFunctions = []

operandStackLimit = None
classSizeLimit = 12000
callTableHierarchy = 1
peepholeIterations = 2

threadSafe = False
saveTemps = False
onlyTranslate = False

debug = False
memoryDebug = False
tracing = False
traceFunctions = None
traceFunctionCalls = False
traceStart = 0
traceEnd = 0

defines = []

threadSafe = False

infile = None
profileFile = None

packageName = ""

def getWtkPath():
    try:
        preverify = which.which("preverify")
    except:
        return "/usr"
    dn = os.path.dirname(preverify)
    base = os.path.dirname(os.path.realpath(dn))

    if base == None:
	base = "/usr/local/share/cibyl"
    return base


def getBasePath():
    dn = os.path.dirname(sys.argv[0])
    base = os.path.dirname(os.path.realpath(dn))

    if base == None:
	base = "/usr/local/share/cibyl"
    return base

wtk = getWtkPath()

jasminCommandLine=getBasePath() + "/bin/cibyl-jasmin"
javacCommandLine="javac -source 1.4 -bootclasspath " + wtk + "/lib/cldcapi11.jar:" + wtk + "/lib/midpapi20.jar"

# setup some of the environment
base_path = getBasePath()
sysroot = base_path + "/mips-cibyl-elf/sys-root/"
jasmin = os.getenv("CIBYL_JASMIN", jasminCommandLine)
javac = os.getenv("CIBYL_JAVAC", javacCommandLine)
readelf = os.getenv("CIBYL_READELF", "readelf")
nm = os.getenv("CIBYL_NM", "nm")
objcopy = os.getenv("CIBYL_OBJCOPY", "objcopy")
cpp = os.getenv("CIBYL_CPP", "cpp")

xcibyl_translator = os.getenv("CIBYL_XCIBYL_TRANSLATOR",
                              base_path + "/bin/xcibyl-translator")

def packageNameJavaPath():
    return "/" + packageName.replace(".", "/")

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
