######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      double_stores.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole template
##
## $Id: double_stores.py 14176 2007-03-11 17:09:26Z ska $
##
######################################################################
import re

from Cibyl.PeepholeOptimizer.template import Template, addTemplate
from Cibyl.PeepholeOptimizer.classes import *


def midInstructionValid(N, item):
    if item.sourceLocal == N or item.targetLocal == N or isinstance(item, BranchInstruction):
	return False
    return True

#	istore $N
#	instruction-1*
#	iload  $N
#	instruction-2*
#	istore $N
# ->
#	instruction-1*
#       instruction-2*
#       istore $N
class MatchStoreLoadStore(Template):
    def __init__(self, n, m):
	pre  = [{"class" : Istore}]
	mid1 = [{"class" : Instruction}]
	load = [{"class" : Iload}]
	mid2 = [{"class" : Instruction}]
	post = [{"class" : Istore}]

	out = pre + mid1 * n + load + mid2 * m + post
	self.midLoadIndex = n+1

	Template.__init__(self, out)

    def match(self, fn, items):
	N = items[0].targetLocal
	if N != items[self.midLoadIndex].sourceLocal:
	    return False
	if N != items[-1].targetLocal:
	    return False
	for item in items[1:self.midLoadIndex]:
	    if not midInstructionValid(N, item):
		return False
	for item in items[self.midLoadIndex+1:-1]:
	    if not midInstructionValid(N, item):
		return False
	return True

    # Remove the istore/iload
    def execute(self, fn, items):
	items[0] = Nop()
	items[self.midLoadIndex] = Nop()
	return items



#	istore $N
#	instruction*
#	istore $N
# ->
#       instruction*
#       istore $N
class MatchStoreStore(Template):
    def __init__(self, n):
	pre = [{"class" : Istore}]
	mid = [{"class" : Instruction}]
	post = [{"class" : Istore}]
	out = pre + mid * n + post

	Template.__init__(self, out)

    def match(self, fn, items):
	N = items[0].targetLocal
	if N != items[-1].targetLocal:
	    return False
	for item in items[1:-1]:
	    if not midInstructionValid(N, item):
		return False
	return True

    # Remove the istore/iload
    def execute(self, fn, items):
	items[0] = Pop()
	return items


# Assuming nothing in the function jumps to LABEL
#	istore $N
# LABEL:
#	iload $N
#       ireturn
# ->
# LABEL:
#       ireturn
class MatchStoreLabelLoad(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Istore},
			   {"class" : Label},
			   {"class" : Iload},
			   {"class" : Ireturn},
			   ])

    def match(self, fn, items):
	if items[0].targetLocal != items[-2].sourceLocal:
	    return False
	# See to it that there are no jumps to LABEL
	for fnItem in fn.contents:
	    if isinstance(fnItem, BranchInstruction):
		if fnItem.target == items[1].name:
		    return False
	return True

    # Remove the istore/iload
    def execute(self, fn, items):
	items[0] = Nop()
	items[-2] = Nop()
	return items

# Assuming nothing in the function jumps to LABEL
#	istore $N
# LABEL:
#       iload  $X != $N
#       putstatic saved_v1
#	iload  $N
#       ireturn
# ->
# LABEL:
#       iload  $X != $N
#       putstatic saved_v1
#       ireturn
class MatchStoreLabelPutstaticLoad(MatchStoreLabelLoad):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Istore},
			   {"class" : Label},
			   {"class" : Iload},
			   {"class" : Putstatic},
			   {"class" : Iload},
			   {"class" : Ireturn},
			   ])

    def match(self, fn, items):
	if not MatchStoreLabelLoad.match(self, fn, items):
	    return False
	N = items[0].targetLocal
	if items[2].sourceLocal == N:
	    return False
	return True

    # Remove the istore/iload
    def execute(self, fn, items):
	items[0] = Nop()
	items[-2] = Nop()
	return items



# Assuming nothing in the function jumps to LABEL
#       dup
#       istore
# LABEL:
#       ireturn
# ->
# LABEL:
#       ireturn
class MatchDupIstoreLabelIreturn(MatchStoreLabelLoad):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Dup},
			   {"class" : Istore},
			   {"class" : Label},
			   {"class" : Ireturn},
			   ])

    def match(self, fn, items):
	# See to it that there are no jumps to LABEL
	for fnItem in fn.contents:
	    if isinstance(fnItem, BranchInstruction):
		if fnItem.target == items[2].name:
		    return False
	return True

    # Remove the dup/istore
    def execute(self, fn, items):
	items[0] = Nop()
	items[1] = Nop()
	return items


#	istore $N
#	iload $N
# ->
#       dup
#       istore $N
class MatchStoreLoad(Template):
    def __init__(self):
	pre = [{"class" : Istore}]
	post = [{"class" : Iload}]
	out = pre + post

	Template.__init__(self, out)

    def match(self, fn, items):
	N = items[0].targetLocal
	if N != items[1].sourceLocal:
	    return False
	return True

    # Remove the istore/iload
    def execute(self, fn, items):
        items[1] = items[0]
	items[0] = Dup()
	return items


#	istore $N
#	iload  $X, $X != $Y
#	iload  $N
#	push-instruction
#	istore $N
# ->
#       iload  $X
#       swap
#	push-instruction
#       istore $N
class MatchSwap(Template):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Istore},
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Instruction},
			   {"class" : Istore},
			   ]
			  )
    def match(self, fn, items):
	N = items[0].targetLocal
	if N == items[1].sourceLocal:
	    return False
	if N != items[2].sourceLocal:
	    return False
	for item in items[3:-1]:
	    if not midInstructionValid(N, item):
		return False
	if items[-1].targetLocal == N:
	    return True
	return False

    # Remove the istore/iload
    def execute(self, fn, items):
	items[0] = Nop()
	items[2] = Instruction("\tswap\n")
	return items

class MatchSwapDup(MatchSwap):
    def __init__(self):
	Template.__init__(self,
			  [{"class" : Istore},
			   {"class" : Iload},
			   {"class" : Iload},
			   {"class" : Instruction},
			   {"class" : Dup},
			   {"class" : Istore},
			   ]
			  )

    def match(self, fn, items):
	N = items[0].targetLocal
	if N == items[1].sourceLocal:
	    return False
	if N != items[2].sourceLocal:
	    return False
	for item in items[3:-1]:
	    if not midInstructionValid(N, item):
		return False
	return True

    # Remove the istore/iload
    def execute(self, fn, items):
	items[0] = Nop()
	items[2] = Instruction("\tswap\n")
	return items

for n in range(0, 1):
    for m in range(0, 6):
	addTemplate(MatchStoreLoadStore(n, m))
for i in range(0, 6):
    addTemplate(MatchStoreStore(i))
addTemplate(MatchStoreLabelLoad())
addTemplate(MatchStoreLoad())
addTemplate(MatchDupIstoreLabelIreturn())
addTemplate(MatchStoreLabelPutstaticLoad())
addTemplate(MatchSwap())
addTemplate(MatchSwapDup())
