######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      const_and_inc.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: getstatic_v1_pop.py 14167 2007-03-11 14:03:38Z ska $
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	getstatic
#	pop
#->
#
class MatchPop(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Getstatic},
			   {"class" : Pop},
			   ]
			  )

    def match(self, fn, items):
	return True

    # make the iinc a nop and replace the const
    def execute(self, fn, items):
	items[0] = Nop()
	items[1] = Nop()
	return items

#	getstatic
#	pop2
#->
#       pop
class MatchPop2(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Getstatic},
			   {"class" : Pop2},
			   ]
			  )

    # make the iinc a nop and replace the const
    def execute(self, fn, items):
	items[0] = Nop()
	items[1] = Pop()
	return items

addTemplate(MatchPop())
addTemplate(MatchPop2())
