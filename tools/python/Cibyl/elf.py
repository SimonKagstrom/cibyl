######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      ElfFile.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Container class for ELF files
##
## $Id: elf.py 12923 2007-01-07 18:12:24Z ska $
##
######################################################################
import re, os, sys, struct
import tempfile

class config:
    pass

# From Dissy
ADR  = "[0-9,a-f,A-F]+"
FN = "(?:[.]*)[_,0-9,a-z,A-Z,\:,\*,\,\(,\), ,<,>]+"

symbolRegexp = re.compile("(" + ADR + ")* *(" + ADR + ")* ([A,B,C,D,G,I,N,R,S,T,U,V,W,a,b,c,d,g,i,n,r,s,t,u,v,w,-,?]{1}) ("+ FN + "){1}")
sectionRegexp = re.compile("[ \t]*\[([ ,0-9]+)\]+[ ,\t]+([A-Z,a-z,0-9,_,\.]+)[ ,\t]+([A-Z,_,\.]+)[ ,\t]+("+ADR+")[ ,\t]+("+ADR+")[ ,\t]+("+ADR+")")
relocationSectionRegexp = re.compile("Relocation section '.rel([\.,A-Z,a-z,0-9,_]+)' at offset")
relocationRegexp = re.compile("(" + ADR + ")[ \t]+(?:" + ADR + ")[ \t]+([a-z,A-Z,0-9_]+)[ \t]+(" + ADR + ")[ \t]+(" + FN + ")")

config.readelf = os.getenv("CIBYL_READELF", "readelf")
config.nm = os.getenv("CIBYL_NM", "nm")
config.objcopy = os.getenv("CIBYL_OBJCOPY", "objcopy")

class Symbol:
    "Container class for symbols"
    def __init__(self, address, size, t, name):
        try:
            self.address = long(address, 16)
        except:
            assert(t == 'U' or t == 'u')
            self.address = 0
	try:
	    self.size = long(size, 16)
	except:
	    self.size = 0
	self.type = t
	if t == "W":
	    self.type = "T"
	self.name = name

    def localLinkage(self):
	return self.type.islower()

    def inTextSection(self):
	return self.type.lower() == "t" or self.type == "W"

    def __str__(self):
	return "%25s at 0x%08x: %s (size %d)" % (self.name, self.address, self.type, self.size)

class Section:
    "Container class for ELF sections"
    def __init__(self, nr, name, t, address, offset, size):
	self.nr = long(nr)
	self.name = name
	self.type = t
	self.address = long(address, 16)
	self.offset = long(offset, 16)
	self.size = long(size, 16)

class Relocation:
    "Container class for relocations"
    def __init__(self, section, offset, t, value, name):
	self.offset = long(offset, 16)
	self.type = t
	self.value = long(value, 16)
	self.name = name
        self.section = section

    def __str__(self):
        return "Relocation %s: %s %x %x t %s" % (self.section, self.name, self.value, self.offset, self.type)

class Elf:
    def __init__(self, filename):
	self.filename = filename
	self.elfHeader = {}
	self.sections = {}
	self.symbols = {}
	self.relocations = []

	# Read the header
	f = os.popen("%s --file-header --wide %s" % (config.readelf, self.filename))
	for line in f:
	    words = line.split(":")
	    if len(words) > 1:
		self.elfHeader[words[0].strip()] = words[1].strip()
	f.close()

	# Read sections
	f = os.popen("%s --section-headers --wide %s" % (config.readelf, self.filename))
	for line in f:
	    match = sectionRegexp.match(line)
	    if match:
		self.sections[match.group(2)] = Section(match.group(1), match.group(2), match.group(3),
							match.group(4), match.group(5), match.group(6))
	f.close()

	# Read symbols
	f = os.popen("%s --numeric-sort --print-size %s" % (config.nm, self.filename))

	tmp = []
	for line in f:
	    match = symbolRegexp.match(line)
	    if match:
		cur = Symbol(match.group(1), match.group(2),
			     match.group(3), match.group(4))
		tmp.append(cur)
		# Store by address
		self.symbols[cur.address] = cur
	f.close()

	# Read relocations
	f = os.popen("%s --relocs --wide %s" % (config.readelf, self.filename))
        sectionName = None
	for line in f:
            if line.startswith("Relocation section "):
                if '.rel.pdr' in line:
                    break
                match = relocationSectionRegexp.match(line)
                if match:
                    sectionName = match.group(1)
	    match = relocationRegexp.match(line)
	    if match:
		self.relocations.append(Relocation(sectionName, match.group(1), match.group(2), match.group(3), match.group(4)))

	f.close()

	# Fixup the symbol sizes
	for sym in tmp[1:]:
	    last = tmp[tmp.index(sym) - 1]
	    if last.size == 0:
		last.size = sym.address - last.address
	last = tmp[-1]
	if last.inTextSection():
	    section = self.sections[".text"]
	    last.size = section.address + section.size - last.address

        self.constructorTable = {}
        self.destructorTable = {}
        self.constructors = []
        self.destructors = []
        ctorsData = self.getSectionContents(".ctors")
	for i in range(0, len(ctorsData), 4):
	    data = ctorsData[i:i+4]
	    word = struct.unpack(">L", data)[0]
            self.constructors.append(word)
            self.constructorTable[word] = True

        dtorsData = self.getSectionContents(".dtors")
	for i in range(0, len(dtorsData), 4):
	    data = dtorsData[i:i+4]
	    word = struct.unpack(">L", data)[0]
            self.destructors.append(word)
            self.destructorTable[word] = True

    def getEntryPoint(self):
	"Get the entry point for the file"
	return long(self.elfHeader["Entry point address"], 16)

    def getSymbolByAddress(self, addr):
        return self.symbols[addr]

    def getSymbolsByType(self, t):
	"Return a list of all symbols of a particular type"
	out = []
	for sym in self.symbols.values():
	    if sym.type.lower() in t:
		out.append(sym)
	return out

    def getSymbolAddress(self, symbolName):
	"Return the address of a particular symbol"
	for sym in self.symbols.values:
	    if sym.name == symbolName:
		return address
	return 0

    def getRelocation(self, name):
	"Return the relocation for a particular name, or None if it doesn't exist"
        for rel in self.relocations:
            if rel.name == name:
                return rel
	return None

    def isConstructor(self, address):
        "Return if this address is in the constructors table"
        return self.constructorTable.has_key(address)

    def isDestructor(self, address):
        "Return if this address is in the destructors table"
        return self.destructorTable.has_key(address)

    def getRelocationsByTargetAddress(self, address):
	"Return the relocations to a particular target address, or None if it doesn't exist"
        return [ rel for rel in self.relocations if rel.value == address ]

    def getRelocationsBySection(self, sectionName):
	"Get the relocations of a particular section"
	sect = self.sections[sectionName]
	return [ rel for rel in self.relocations if rel.offset >= sect.address and rel.offset < sect.address + sect.size ]

    def getSectionAddress(self, sectionName):
	return self.sections[sectionName].address

    def getSectionContents(self, sectionName):
	"Get contents of a named section"
	if not self.sections.has_key(sectionName):
	    return ""
	section = self.sections[sectionName]
	tmp = tempfile.mkstemp()
	os.system('%s -j "%s" -O binary %s %s' % (config.objcopy, sectionName, self.filename, tmp[1]))
	f = open(tmp[1])
	out = f.read()
	f.close()
	os.remove(tmp[1])
	return out

    def getSections(self):
	"Get all sections"
	return self.sections

    def getSectionSize(self, sectionName):
	"Get size of a named section"
	return self.sections[sectionName].size

    def getSectionAddress(self, sectionName):
	"Get address of a named section"
	return self.sections[sectionName].address


if __name__ == "__main__":
    e = Elf(sys.argv[1])
    print ".text from 0x%08x to %08x" % (e.getSectionAddress(".text"),
					 e.getSectionAddress(".text") + e.getSectionSize(".text"))
    for v in e.getSymbolsByType("t"):
	print v

