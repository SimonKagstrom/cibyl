######################################################################
##
## Filename:      push_pop_inc.py
## Author:        Marcus Groeber <marcus.groeber@nuance.com>
## Description:   Peephole template
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	push $X != $Z
#	pop $Y != $Z
#	iinc $N $Z
#->
#	iinc $N $Z
#	push $X != $Z
#	pop $Y != $Z
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : PushInstruction},
			   {"class" : Istore},
			   {"class" : Iinc},
			   ]
			  )

    def match(self, fn, items):
	if items[0].sourceLocal == items[2].targetLocal or items[1].targetLocal == items[2].targetLocal:
	    return False
	return True

    # Percolate up "iinc" to prepare for merging with constants, if possible
    def execute(self, fn, items):
	temp = items[2]
	items[2] = items[1]
	items[1] = items[0]
	items[0] = temp
	return items

addTemplate(MatchClass())
