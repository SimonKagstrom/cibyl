######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      goto_next_line.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: goto_next_line.py 14168 2007-03-11 14:03:44Z ska $
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	goto LABEL
#LABEL:
#->
#LABEL:
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Goto},
			   {"class" : Label},
			   ]
			  )

    def match(self, fn, items):
	if items[0].target != items[1].name:
	    return False
	return True

    # The action here is simple - just remove the instruction
    def execute(self, fn, items):
	items[0] = Nop()
	return items # Keep the label

addTemplate(MatchClass())
