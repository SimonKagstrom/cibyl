######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      translator.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Describes a MIPS binary
##
## $Id: translator.py 14248 2007-03-14 17:13:31Z ska $
##
######################################################################
import StringIO, re, sys, struct
from sets import Set

from Cibyl.BinaryTranslation import bytecode, register, function, codeblock
from Cibyl.BinaryTranslation.Mips.instruction import Instruction, newInstruction, Syscall, SyscallRegisterArgument
from Cibyl.BinaryTranslation.Mips import mips, optimizer, javamethod

from Cibyl import config

import Cibyl.SyscallHandling.function, Cibyl.elf

syscallDefinitionRegexp = re.compile("[ \t]*static inline _syscall([0-9]+)\(([A-Z,a-z,0-9,_,\*]+)[ \t\,]*,([A-Z,a-z,0-9,_]+)[ \t]*([A-Z,a-z,0-9,\*,\,, ,_]*)\);")


def getSyscallStrings(data):
    """Split the .cibylstrtab section into address : string mappings
    (and the other way around"""
    addressesToName = {}
    strs = data.split("\0")

    cur = 0
    for s in strs:
	addressesToName[cur] = s
	cur = cur + len(s) + 1
    return addressesToName


class Controller(codeblock.CodeBlock):
    def __init__(self, filename, syscallDirectories):
	self.filename = filename
	self.syscallDirectories = syscallDirectories
	self.tracing = config.tracing
	self.labels = {}
	self.instructions = []
	self.instructionsByAddress = {}
	self.syscalls = {}
	self.usedSyscalls = {}
	self.outclass = "CompiledProgram"
	self.compileFunctions = True

	self.optimizer = optimizer.Optimizer()
	self.registerHandler = register.RegisterHandler(self, bytecode.ByteCodeGenerator(self))
	self.elf = Cibyl.elf.Elf(self.filename)

	self.addressesToName = getSyscallStrings(self.elf.getSectionContents(".cibylstrtab"))

	# Read everything
	allFunctions = Cibyl.SyscallHandling.function.functionsFromHeaderDirectories(syscallDirectories)
	for fn in allFunctions:
	    self.addSyscall(fn)
	self.readBinary()

	for insn in self.instructions:
	    insn.fixup()

	# Fixup the jump destinations and other stuff with the instructions
	for insn in self.instructions:
            self.instructionsByAddress[insn.address] = insn
	    if insn.delayed:
                assert(not self.instructionsByAddress.has_key(insn.delayed.address))
                self.instructionsByAddress[insn.delayed.address] = insn.delayed
		insn.delayed.fixup()
	    if insn.prefix:
                assert(not self.instructionsByAddress.has_key(insn.prefix.address))
                self.instructionsByAddress[insn.prefix.address] = insn.prefix
		insn.prefix.fixup()
	    if insn.address in self.labels:
		insn.isBranchDestination = True

	# Arrange instructions into functions
	self.functions = []
	for sym in self.elf.getSymbolsByType("tW"):
	    # Add a label for this address and create a new function
	    self.addLabel(sym.address, inJumpTab = True, name = sym.name, isFunction = True)
	    insns, labels = self.splitByAddresses(sym.address, sym.address + sym.size)

	    # Maybe trace this function
	    useTracing = self.tracing
	    if config.traceFunctions:
		useTracing = sym.name in config.traceFunctions

	    fn = function.Function(self, sym.name, insns, labels, useTracing)

	    # If this function is unused, remove all references to it
	    if config.pruneUnusedFunctions and not self.elf.getRelocation(sym.name) and not sym.type in "t" and sym.address != self.elf.getEntryPoint():
		if config.verbose: print "Pruning ", sym.name
		for insn in insns:
		    self.removeLabel(insn.address)
		continue
	    self.functions.append(fn)

	self.functions.sort()
	self.javaMethods = []

        colocateFunctions = []
        otherFunctions = []
        for fn in self.functions:
            if fn.name in config.colocateFunctions:
                colocateFunctions.append(fn)
            else:
                otherFunctions.append(fn)

        # Insert java methods (colocated and normal)
        if colocateFunctions != []:
            for fn in colocateFunctions:
                for insn in fn.instructions:
                    if isinstance(insn, Syscall):
                        self.markSyscallUsed( self.addressesToName[insn.extra] )
            self.javaMethods.append(javamethod.JavaMethod(self, colocateFunctions))

	for fn in otherFunctions:
	    for insn in fn.instructions:
		if isinstance(insn, Syscall):
		    self.markSyscallUsed( self.addressesToName[insn.extra] )
	    self.javaMethods.append(javamethod.JavaMethod(self, [fn]))
	jumptab = javamethod.GlobalJumptabMethod(self, self.javaMethods[:])
	self.javaMethods.append(jumptab)

	if config.doRelocationOptimization:
	    c = 0
	    for label in self.labels.values():
		if label.inJumpTab:
		    label.address = c
		    c = c + 1

	# Finally create the outfile
	if config.outFilename == None:
	    self.outfile = sys.stdout
	else:
	    self.outfile = StringIO.StringIO()

    def getOptimizer(self):
	"Return the optimizer"
	return self.optimizer

    def getInstruction(self, address):
        return self.instructionsByAddress[address]

    def markSyscallUsed(self, name):
	"Mark syscall nr as used"
	self.usedSyscalls[name] = True

    def addSyscall(self, func):
	"Add a syscall for a given function"
	self.syscalls[func.name] = func

    def hasSyscall(self, address):
	"Return the syscall on the address @a address"
	name = self.addressesToName[address]
	return self.syscalls[name]

    def usesSyscall(self, name):
	"Check if a named syscall is actually used"
	return self.usedSyscalls.has_key(name)

    def writeAssemblyFile(self, filename):
	"Output the Java assembly file"
	self.outfile.flush()
	f = open(filename, "w")
	f.write(self.outfile.getvalue())
	f.close()

    def lookupJavaMethod(self, address):
	"Return the java method for a given address"
	for method in self.javaMethods:
            for fn in method.functions:
                if address in (fn.address, fn.address + fn.size - 1):
                    return method
	return None

    def addAlignedSection(self, out, sectionName, alignment):
	section = self.elf.getSectionContents(sectionName)
	addr = len(out)
	pad = 0
	if (addr & (alignment-1)):
	    pad = ((-addr) & (alignment-1))
	out = out + "\0" * pad
	out = out + section
	return out

    def writeDataFile(self, filename):
	"Output the raw .data/.rodata file"
	sections = self.addAlignedSection("", ".data", 16)
	sections = self.addAlignedSection(sections, ".rodata", 16)
	sections = self.addAlignedSection(sections, ".ctors", 4)
	sections = self.addAlignedSection(sections, ".dtors", 4)

	# Write the data outfile
	out = open(filename, "w")
	out.write(sections)
	out.close()


    # Based on Instruction.java by
    # George Fankhauser <gfa@acm.org>
    def readBinary(self):
	"Parse the binary file and create the instructions"

	baseAddress = self.elf.getEntryPoint()
	text = self.elf.getSectionContents(".text")
	rodata = self.elf.getSectionContents(".rodata") + self.elf.getSectionContents(".ctors") + self.elf.getSectionContents(".dtors")

	# Add all .rodata refs to the .text segment to the lookup table
	for i in range(0, len(rodata), 4):
	    data = rodata[i:i+4]
	    word = struct.unpack(">L", data)[0]

	    if word >= baseAddress and word < baseAddress + len(text):
		self.addLabel(word, True)

	# Parse the instructions
	i = 0
	delaySlot = None
	while i < len(text):
	    data = text[i:i+4]
	    word = struct.unpack(">L", data)[0]

	    address = i + baseAddress

	    extra = 0
	    rs = (word >> 21) & 0x1f
	    rt = (word >> 16) & 0x1f
	    rd = (word >> 11) & 0x1f
	    opEntry = mips.opTable[(word >> 26) & 0x3f]
	    opCode = opEntry[mips.OPCODE]
	    format = mips.SPECIAL

	    # The special encodings are handled here
	    if (word >> 16) == 0xfefe or (word >> 24) == 0xff:
		if (word >> 24) == 0xff:    # Syscall
		    extra = word & 0x00ffffff
		    insn = Syscall(self, address, mips.CIBYL_SYSCALL, opCode, 0,0,0, extra)
		elif (word >> 16) == 0xfefe:  # Register-parameter
		    extra = word & 0xffff
		    insn = SyscallRegisterArgument(self, address, mips.CIBYL_REGISTER_ARGUMENT, opCode, 0,0,0, extra)
		else:
		    raise Exception("Unknown special encoding %x" % long(word))
		self.addInstruction(insn)
		i = i + 4
		continue

	    if opEntry[mips.FORMAT] == mips.IFMT:
		format = mips.IFMT
		extra = word & 0xffff
		# Negative?
		if opCode not in mips.ifmtZeroExtend and extra & 0x8000 == 0x8000:
		    extra = (-1 & ~0xffff) | extra

	    elif opEntry[mips.FORMAT] == mips.RFMT:
		format = mips.RFMT
		extra = (word >> 6) & 0x1f
	    else: # JFMT
		format = mips.JFMT
		extra = word & 0x03ffffff

	    if opCode == mips.SPECIAL:
		opCode = mips.specialTable[word & 0x3f]
	    elif opCode == mips.BCOND:
		x = word & 0x1f0000
		if x == 0:
		    opCode = mips.OP_BLTZ
		elif x == 0x10000:
		    opCode = mips.OP_BGEZ
		elif x == 0x100000:
		    opCode = mips.OP_BLTZAL
		elif x == 0x110000:
		    opCode = mips.OP_BGEZAL
		else:
		    opCode = mips.OP_UNIMP

	    insn = newInstruction(opCode)(self, address, format, opCode, rd, rs, rt, extra)
	    # If the last instruction has a delay slot, add this to
	    # it, otherwise add it to the lits of instructions
	    if delaySlot:
		delaySlot.addDelayedInstruction(insn)
		delaySlot = None
	    else:
		self.addInstruction(insn)
	    if insn.opCode in mips.delaySlotInstructions:
		delaySlot = insn
	    i = i + 4

    def compile(self):
	"Recompile the binary"
	if config.debug:
	    self.emit(".source CompiledProgram.j")
	self.emit(".class public %s" % (self.outclass) )
	self.emit(".super java/lang/Object")

	for i in range(0,3):
	    self.emit('')

	for method in self.javaMethods:
	    method.fixup()

	for method in self.javaMethods:
	    method.fixup()

	for method in self.javaMethods:
	    method.compile()
	    for i in range(0,3):
		self.emit('')

    def emit(self, what):
	"Emit instructions"
	self.outfile.write(what + "\n")
