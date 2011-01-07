######################################################################
##
## Copyright (C) 2008,  Simon Kagstrom
##
## Filename:      helpers.py
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Helpers for the C++ translator
##
## $Id:$
##
######################################################################
import os, sys
import Cibyl.PeepholeOptimizer.parse

from Cibyl import config

def doJasmin(filename):
    f = filename
    if config.doPeepholeOptimize:
        Cibyl.PeepholeOptimizer.parse.run(config.peepholeIterations, f, f)
    ret = os.system(config.jasmin + " -d " + config.outDirectory + " " + f)
    if ret != 0:
        sys.exit(ret)

def doJavac(filename):
    ret = os.system(config.javac + " -d " + config.outDirectory +
                    " -classpath " + config.outDirectory + " " + filename )
    if ret != 0:
        sys.exit(ret)


def doTranslation(filename, syscallDirectories):
    dbs = ""
    defines = " "
    conf = "config:"

    if config.traceStart != 0:
        conf = conf + "trace_start=0x%x," % (config.traceStart)
    if config.traceEnd != 0:
        conf = conf + "trace_end=0x%x," % (config.traceEnd)
    if config.memoryDebug:
        conf = conf + "trace_stores=1,"
    if config.doOptimizeIndirectCalls:
        conf = conf + "prune_call_table=1,"
    if config.doOptimizePruneStackStores:
        conf = conf + "optimize_prune_stack_stores=1,"
    if config.doOptimizeFunctionArguments:
        conf = conf + "optimize_function_return_arguments=1,"
    if config.threadSafe:
        conf = conf + "thread_safe=1,"
    if len(config.colocateFunctions) > 0:
        l = len(config.colocateFunctions)
        s = ""
        for i in config.colocateFunctions[:l-1]:
            s = s + i + ";"
        s = s + config.colocateFunctions[-1]
        conf = conf + "colocate_functions=" + s + ","
    if config.pruneUnusedFunctions:
        conf = conf + "prune_unused_functions=1,"
    else:
        conf = conf + "prune_unused_functions=0,"
    conf = conf + "class_size_limit=" + str(config.classSizeLimit) + ","
    conf = conf + "call_table_hierarchy=" + str(config.callTableHierarchy) + ","
    conf = conf + "call_table_classes=" + str(config.callTableClasses) + ","
    if config.packageName != "":
        conf = conf + "package_name=" + config.packageName

    for d in config.defines:
        defines = defines + d + " "

    for d in syscallDirectories:
        dbs = dbs + d + "/cibyl-syscalls.db "

    call_str = config.xcibyl_translator + " \"" + conf + "\" " + defines + config.outDirectory + " " + config.infile + " " + dbs
    ret = os.system(call_str)
    if ret != 0:
        sys.exit(ret)
