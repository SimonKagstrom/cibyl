######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      template.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Peephole templates
##
## $Id: template.py 14163 2007-03-11 14:01:05Z ska $
##
######################################################################
from classes import Label

templates = []

def addTemplate(t):
    templates.append(t)

def getTemplates():
    return templates

class Template:
    def __init__(self, matches):
	self.matches = matches

    def execute(self, items):
	pass

    def match(self, fn, items):
	return False

    def apply(self, fn):
	matching = 0
	first = 0
	cur = 0
	applied = 0

	while cur < len(fn.contents):
	    item = fn.contents[cur]
	    if item.isNop:
		pass
	    elif isinstance(item, self.matches[matching]["class"]):
	        if matching == 0:
		    first = cur
		matching = matching + 1
                if matching == len(self.matches):
		    # We found a match
		    start = first
		    end = cur+1
		    # Get the things to check - skip nops
		    test = []
		    for item in fn.contents[start : end]:
		        if not item.isNop:
			    test.append(item)
			    
		    # Check that it's actually valid
		    if self.match(fn, test):
		        replacement = self.execute( fn, test )
		        applied = applied + 1
		        # Perform the actual replacement - skip nops
		        j = 0
		        for i in range(start, end):
			    if fn.contents[i].isNop:
			        continue
			    fn.contents[i] = replacement[j]			
			    j = j + 1
		    # Go back to one after start of match
		    cur = first
		    matching = 0
	    elif matching > 0:
	        cur = first
		matching = 0

	    cur = cur + 1

        return applied