######################################################################
##
## Copyright (C) 2007,  Simon Kagstrom
##
## Filename:	  profile.py
## Author:		Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Parsing of a J2ME .prf file
##
## $Id:$
##
######################################################################
import re, copy, sys

findRegexp = re.compile("[ ,\t,a-z,A-Z,0-9,_,\.]+Cibyl[0-9]*\.")
functionNameRegexp = re.compile("Cibyl[0-9]*\.([a-z,A-Z,_,0-9]+)")

class Entry:
	def __init__(self, entries):
		self.index = int(entries[0])
		self.parent = int(entries[1])
		self.depth = int(entries[2])
		self.name = entries[3]
		self.count = int(entries[4])
		self.cycles = int(entries[5])
		self.msec = int(entries[6])
		# Skip the rest for now

class Profile:
	def __init__(self, data):
		self.entries = {}
		self.sortByCount = []
		self.sortByCycles = []

		for line in data.splitlines():
			if not findRegexp.match(line):
				continue
			# Found a Cibyl method
			self.addEntry(line)
		if self.entries == {}:
			raise Exception("No Cibyl profile found")
		self.sortByCount.sort()
		self.sortByCount.reverse()
		self.sortByCycles.sort()
		self.sortByCycles.reverse()

	def addEntry(self, line):
		class SortByCount(Entry):
			def __cmp__(self, other):
				return cmp(self.count, other.count)
		class SortByCycles(Entry):
			def __cmp__(self, other):
				return cmp(self.cycles, other.cycles)

		entries = line.split()
		entries[3] = functionNameRegexp.match(entries[3]).group(1)
		p = Entry(entries)
		self.entries[p.name] = p
		self.sortByCount.append(SortByCount(entries))
		self.sortByCycles.append(SortByCycles(entries))

	def getEntryByName(self, name):
		return self.entries[name]

	def getEntriesSortedByCallCount(self):
		return self.sortByCount


if __name__ == "__main__":
	f = open(sys.argv[1])
	d = f.read()
	f.close()
	pr = Profile(d)
	c = pr.getEntriesSortedByCallCount()
	# Print entries sorted by call-count
	for e in c:
		print e.count, e.name
