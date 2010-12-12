######################################################################
##
## Filename:      unused_label.py
## Author:        Marcus Groeber <marcus.groeber@nuance.com>
## Description:   Peephole template
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#Assuming nothing jumps to __CIBYL_<label>:
#	__CIBYL_<label>:
#->
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Label}]
			  )

    def match(self, fn, items):
	if not items[0].name.startswith("__CIBYL"):
	    return False
	# See to it that there are no jumps to LABEL
	for fnItem in fn.contents:
	    if isinstance(fnItem, BranchInstruction):
		if fnItem.target == items[0].name:
		    return False
	    if fnItem.line.startswith(".catch"):
		return False
	return True

    # Percolate up "iinc" to prepare for merging with constants, if possible
    def execute(self, fn, items):
	items[0] = Nop()
	return items

addTemplate(MatchClass())
