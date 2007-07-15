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

	matches = []

	for item in fn.contents:

	    if isinstance(item, self.matches[matching]["class"]):
		if matching == 0:
		    first = cur
		matching = matching + 1
	    elif not item.isNop:
		matching = 0

	    if matching == len(self.matches):
		# We found a match
		matches.append( (first, cur+1) )
		matching = 0

	    cur = cur + 1

	for t in matches:
	    start, end = t

	    # Get the things to check - skip nops
	    test = []
	    for item in fn.contents[start : end]:
		if not item.isNop:
		    test.append(item)

	    # Check that it's actually valid
	    if not self.match(fn, test):
		continue

	    replacement = self.execute( fn, test )
	    # Perform the actual replacement - skip nops
	    j = 0
	    for i in range(start, end):
		if fn.contents[i].isNop:
		    continue
		fn.contents[i] = replacement[j]
		j = j + 1

