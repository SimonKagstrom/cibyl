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

from template import Template, addTemplate
from classes import *

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
    def __init__(self):
	Template.__init__(self,
			  [
			   {"class" : Iload},
			   ]
			  )

    def match(self, fn, items):
	return False

    def execute(self, items):
	return items

addTemplate(MatchBase())

