######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      jal_return.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: jal_arguments.py 14138 2007-03-10 18:31:22Z ska $
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	getstatic ...
#	istore 6
#	istore 5
#	iload_0
#	iload_1
#	iload_2
#	iload_3
#	iload_4
#	invokestatic ...
#	getstatic ...
#	istore 6
#	istore 5
# ->
#	pop
#	iload_0
#	iload_1
#	iload_2
#	iload_3
#	iload_4
#	invokestatic ...
#	getstatic ...
#	istore 6
#	istore 5

class MatchJalReturn(Template):
    def __init__(self, n, m):
	Template.__init__(self,
			  [
			   {"class" : Getstatic},
			   {"class" : Istore},
			   {"class" : Istore},
			   ] + [{"class" : PushInstruction}, {"class" : PopInstruction}] * n + [{"class" : PushInstruction}] * m + [
			   {"class" : Invokestatic},
			   {"class" : Getstatic},
			   {"class" : Istore},
			   {"class" : Istore},
			   ]
			  )

    def match(self, fn, items):
      regs = [items[1].targetLocal, items[2].targetLocal]
      if not items[-1].targetLocal in regs or not items[-2].targetLocal in regs:
        return False
      i = 3
      while not isinstance(items[i], Getstatic):
          if items[i].sourceLocal and items[i].sourceLocal in regs:
              return False
          i += 1
      return True

    def execute(self, fn, items):
        items[0] = Nop()
        items[1] = Nop()
        items[2] = Pop()
        return items

for n in range(0, 5):
    for m in range(3, 6):
        addTemplate(MatchJalReturn(n, m))
