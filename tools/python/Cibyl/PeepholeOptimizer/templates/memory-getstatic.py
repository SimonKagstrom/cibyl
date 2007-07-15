######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      goto_next_line.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: memory-getstatic.py 14142 2007-03-10 18:54:28Z ska $
##
######################################################################
import re

from template import Template, addTemplate
from classes import *

#	getstatic CRunTime/memory [I
#	iload 18
#	iconst_2
#	iushr
#	bipush 8
#	iadd
#	iaload
#	istore 5
#	getstatic CRunTime/memory [I
#	iload 18
#	iconst_2
#	iushr
#	bipush 10
#	iadd
#	iaload
#	istore_2
# ->
#	getstatic CRunTime/memory [I
#       dup
#	iload 18
#	iconst_2
#	iushr
#	bipush 8
#	iadd
#	iaload
#	istore 5
#	iload 18
#	iconst_2
#	iushr
#	bipush 10
#	iadd
#	iaload
#	istore_2
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Getstatic, "match" : "getstatic CRunTime/memory [I"},
			   {"class" : Iload},
			   {"class" : Iconst},
			   {"class" : Iushr},
			   {"optional-class" : Iconst},
			   {"optional-class" : Iadd},
			   {"class" : Iaload},
			   {"class" : Istore},
			   ]
			  )

    def match(self, fn, items):
	if items[0].target != items[1].name:
	    return False
	return True

    # The action here is simple - just remove the instruction
    def execute(self, items):
	items[0] = Nop()
	return items # Keep the label

addTemplate(MatchClass())
