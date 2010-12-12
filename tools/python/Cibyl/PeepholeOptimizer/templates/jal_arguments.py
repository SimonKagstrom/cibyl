######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      double_stores.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: jal_arguments.py 14138 2007-03-10 18:31:22Z ska $
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *

#	iload $X
#	istore_1
#	iload $Y
#	istore_2
#       ...
#       instruction*
#	iload_0
#	iload_1
#	iload_2
#       ...
#       jal     ....
# ->
#       iload $X
#       iload $Y
#       ...
#       jal     ....

class MatchBase(Template):
    def __init__(self, n):
	Template.__init__(self,
			   [{"class" : PushInstruction}, {"class" : Istore}] * n +
			   [
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Invokestatic},
			   ]
			  )
	self.n = n

    def match(self, fn, items):
	if not items[-2].sourceLocal=="4" or not items[-3].sourceLocal=="3" or not items[-4].sourceLocal=="2" or not items[-5].sourceLocal=="1" or not items[-6].sourceLocal=="0":
	    return False
	for p in range(1,self.n+1):
	    if items[p*2-2].sourceLocal and int(items[p*2-2].sourceLocal)<=4:
	        return False	
	    if int(items[p*2-1].targetLocal)>4:
	        return False
	return True

    def execute(self, fn, items):
	for p in range(1,self.n+1):
	    reg = int(items[p*2-1].targetLocal)
	    items[-6 + reg] = items[p*2-2]
	    items[p*2-2] = Nop()
	    items[p*2-1] = Nop()
	return items

class MatchBase2(Template):
    def __init__(self, n):
	Template.__init__(self,
			   [{"class" : PushInstruction}, {"class" : Istore}] * n +
			   [
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Invokestatic},
			   ]
			  )
	self.n = n

    def match(self, fn, items):
	if not items[-2].sourceLocal=="3" or not items[-3].sourceLocal=="2" or not items[-4].sourceLocal=="1" or not items[-5].sourceLocal=="0":
	    return False
	for p in range(1,self.n+1):
	    if items[p*2-2].sourceLocal and int(items[p*2-2].sourceLocal)<=3:
	        return False	
	    if int(items[p*2-1].targetLocal)>3:
	        return False
	return True

    def execute(self, fn, items):
	for p in range(1,self.n+1):
	    reg = int(items[p*2-1].targetLocal)
	    items[-5 + reg] = items[p*2-2]
	    items[p*2-2] = Nop()
	    items[p*2-1] = Nop()
	return items

addTemplate(MatchBase(4))
addTemplate(MatchBase(3))
addTemplate(MatchBase(2))
addTemplate(MatchBase(1))

addTemplate(MatchBase2(3))
addTemplate(MatchBase2(2))
addTemplate(MatchBase2(1))
