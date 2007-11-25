######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      const_and_inc.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: const_and_inc.py 14164 2007-03-11 14:01:14Z ska $
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	const  $X
#	pop
#->
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Const},
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

addTemplate(MatchClass())
