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
#	istore_$N
#	iinc $N $Y
#->
#	const $X + $Y
#	istore_$N
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Const},
			   {"class" : Istore},
			   {"class" : Iinc},
			   ]
			  )

    def match(self, fn, items):
	if items[1].targetLocal != items[2].targetLocal:
	    return False
	return True

    # make the iinc a nop and replace the const
    def execute(self, fn, items):
	items[0] = Const(items[0].value + items[2].value)
	items[2] = Nop()
	return items

addTemplate(MatchClass())
