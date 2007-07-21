######################################################################
##
## Copyright (C) 2007,  Simon Kagstrom
##
## Filename:      dup_pop.py
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Part of the peephole optimizer
##
## $Id:$
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	dup
#	pop
# ->
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Dup},
			   {"class" : Pop},
			   ]
			  )

    def match(self, fn, items):
	return True

    def execute(self, fn, items):
	items[0] = Nop()
	items[1] = Nop()
	return items

addTemplate(MatchClass())
