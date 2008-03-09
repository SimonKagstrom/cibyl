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

from Cibyl.BinaryTranslation.translator import config

def doJasmin(filename):
    ret = os.system(config.jasmin + " -d " + config.outDirectory + " " + filename)
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

    for d in config.defines:
        defines = defines + d + " "

    for d in syscallDirectories:
        dbs = dbs + d + "/cibyl-syscalls.db "

    ret = os.system(config.xcibyl_translator + " " + str(config.traceStart) + " " +
                    str(config.traceEnd) +  " " + defines + config.outDirectory + " " +
                    config.infile + " " + dbs)
    if ret != 0:
        sys.exit(ret)
