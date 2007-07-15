######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      const_and_inc.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: putstatic_getstatic.py 14174 2007-03-11 14:52:57Z ska $
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

# With no more getstatic of the same register in the function
#	putstatic lo/hi
#	getstatic lo/hi
#->
#
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Putstatic},
			   {"class" : Getstatic},
			   ]
			  )

    def match(self, fn, items):
	if items[0].targetLocal != items[1].sourceLocal:
	    return False
	N = items[0].targetLocal
	if N != "CRunTime/hi" and N != "CRunTime/lo":
	    return False
	for fnItem in fn.contents:
	    if fnItem in items:
		continue
	    if isinstance(fnItem, Getstatic) and fnItem.sourceLocal == N:
		return False
	return True

    # Just cancel these
    def execute(self, fn, items):
	items[0] = Nop()
	items[1] = Nop()
	return items

addTemplate(MatchClass())
