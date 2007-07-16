######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:      binary.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Parse the MIPS binaries
##
## $Id: binary.py 12825 2006-12-22 16:19:15Z ska $
##
######################################################################
def read(elf, controller):
    """Parse the binary file and create the instructions. Returns a
    tuple of (instructions, labels)"""

    labels = []
    instructions = []

    baseAddress = elf.getEntryPoint()
    text = elf.getSectionContents(".text")
    rodata = elf.getSectionContents(".rodata") + elf.getSectionContents(".ctors") + elf.getSectionContents(".dtors")

    # Add all .rodata refs to the .text segment to the lookup table
    for i in range(0, len(rodata), 4):
	data = rodata[i:i+4]
	word = struct.unpack(">L", data)[0]

	if word >= baseAddress and word < baseAddress + len(text):
	    labels.append(Label(word, True))

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
	if (word >> 20) == 0xfff:
	    extra = word & 0xffff
	    if (word >> 16) == 0xffff:    # Syscall
		insn = Syscall(controller, address, mips.CIBYL_SYSCALL, opCode, 0,0,0, extra)
	    elif (word >> 16) == 0xfffe:  # Register-parameter
		insn = SyscallRegisterArgument(controller, address, mips.CIBYL_REGISTER_ARGUMENT, opCode, 0,0,0, extra)
	    else:
		raise Exception("Unknown special encoding %x" % long(word))
	    instructions.append(insn)
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

	insn = newInstruction(opCode)(controller, address, format, opCode, rd, rs, rt, extra)
	# If the last instruction has a delay slot, add this to
	# it, otherwise add it to the lits of instructions
	if delaySlot:
	    delaySlot.addDelayedInstruction(insn)
	    delaySlot = None
	else:
	    instructions.append(insn)
	if insn.opCode in mips.delaySlotInstructions:
	    delaySlot = insn
	i = i + 4

    return (instructions, labels)
