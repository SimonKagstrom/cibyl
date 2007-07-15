######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      const_and_inc.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: double_pop.py 14165 2007-03-11 14:01:20Z ska $
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	pop
#	pop
#->
#	pop2
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Pop},
			   {"class" : Pop},
			   ]
			  )

    def match(self, fn, items):
	return True

    # make the iinc a nop and replace the const
    def execute(self, fn, items):
	items[0] = Pop2()
	items[1] = Nop()
	return items

addTemplate(MatchClass())
