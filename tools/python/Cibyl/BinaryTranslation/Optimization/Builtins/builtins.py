######################################################################
##
## Copyright (C) ,  Simon Kagstrom
##
## Filename:      builtins.py
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:
##
## $Id:$
##
######################################################################
alwaysInline = []
builtins = []

def addBuiltin(item):
    builtins.append(item)

def match(controller, instruction, name):
    "Match the builtins (inlined functions, e.g., for the softfloat support)"
    for item in builtins:
        ret = item(controller, instruction, name)
        if ret != None:
            return ret
    return None

import softfloat, exceptions
