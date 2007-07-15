######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      config.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Configuration
##
## $Id: config.py 13856 2007-02-24 08:48:25Z ska $
##
######################################################################
import os

verbose = False
outFilename = None
pruneUnusedFunctions = True
doConstantPropagation = False
doMultOptimization = False
doMemoryRegisterOptimization = False
doRelocationOptimization = False
doRegisterScheduling = False
doOptimizeIndirectCalls = False
doInlineAllBuiltins = False

inlineBuiltinsFunctions = []

colocateFunctions = []

debug = False
tracing = False
traceFunctions = None
traceFunctionCalls = False

def getBasePath():
    base = os.getenv("CIBYL_BASE")
    if base == None:
	base = "/usr/local/share/cibyl"
    return base
