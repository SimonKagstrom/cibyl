######################################################################
##
## Copyright (C) 2007,  Simon Kagstrom
##
## Filename:      const_store_aload.py
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Part of the peephole optimizer
##
## $Id:$
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	const xx
#	store $N
#       aload $Y
# ->
#       aload $Y
#	const xx
#	store $N
class MatchClass(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Const},
			   {"class" : Istore},
			   {"class" : Aload},
			   ]
			  )

    def match(self, fn, items):
	return True

    def execute(self, fn, items):
	tmp = items[0]
	tmp2 = items[1]
	items[0] = items[2]
	items[1] = tmp
	items[2] = tmp2
	return items

addTemplate(MatchClass())
