######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      packets.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Handling of GDB packets for the Cibyl GDB server.
##
## $Id:$
##
######################################################################
import jdb, gdbmips
from Queue import Queue, Empty

EMPTY=""

class PacketException(Exception):
    pass

def unsigned(val):
    return long(val) & 0xffffffffl

def qPacket(s):
    if s == "C":
        # Thread id
        return EMPTY
    return EMPTY

def hPacket(s):
    "Set thread for operations"
    return EMPTY

def pPacket(s):
    "Read register packet"
    reg = int(s, 16)
    try:
        val = jdb.readRegister(reg)
    except jdb.JdbException, inst:
        return "E00"
    return "%08x" % unsigned(val)

def PPacket(s):
    "Write register packet"
    reg = int(s[0:s.find("=")], 16)
    value = int(s[s.find("="):], 16)
    try:
        val = jdb.writeRegister(reg, value)
    except jdb.JdbException, inst:
        return "E00"
    return "OK"

def gPacket(s):
    out = ""
    # gp registers
    for i in range(0, 32):
        out = out + pPacket("%02x", i)
    out = out + "%08x" % 0    # sr
    out = out + pPacket("%02x", gdbmips.R_LO)
    out = out + pPacket("%02x", gdbmips.R_HI)
    out = out + "%08x" % 0    # bad
    out = out + "%08x" % 0    # cause
    out = out + pPacket("%02x", gdbmips.R_PC)
    # f-registers
    for i in range(0, 32):
        out = out + "%08x" % 0# f-regs
    out = out + "%08x" % 0    # fsr
    out = out + "%08x" % 0    # fir
    out = out + "%08x" % 0    # fp (hmm...)

def haltPacket(s):
    "? packet, get halt reason"
    return "T00"

def mPacket(s):
    "read memory"
    addr = int(s[:s.find(",")], 16)
    length = int(s[s.find(",")+1:], 16)

    try:
        if addr in range(gdbmips.elf.getSectionAddress(".text"),
                         gdbmips.elf.getSectionAddress(".text") + gdbmips.elf.getSectionSize(".text")):
            data = gdbmips.readMemory(addr, length)
        else:
            data = jdb.readMemory(addr, length)
    except jdb.JdbException, inst:
        return "E00"

    out = ""
    for v in range(0, len(data)):
        out = out + "%02x" % data[v]
    return out

def MPacket(s):
    "write memory"
    addr = int(s[:s.find(",")], 16)
    length = int(s[s.find(",")+1:s.find(":")], 16)
    value = s[s.find(":")+1:]

    try:
        if addr in range(gdbmips.elf.getSectionAddress(".text"),
                         gdbmips.elf.getSectionAddress(".text") + gdbmips.elf.getSectionSize(".text")):
            return "E00"
        else:
            data = jdb.writeMemory(addr, length, value)
    except jdb.JdbException, inst:
        return "E00"
    return "OK"

def splitBreakpointPacket(s):
    split = s.split(",")
    bpType = int(split[0], 16)
    addr = int(split[1], 16)
    length = int(split[2], 16)
    return (bpType, addr, length)

def ZPacket(s):
    "Breakpoint packets"
    bpType, addr, length = splitBreakpointPacket(s)

    try:
        jdb.setBreakpoint( gdbmips.convertAddressToLine(addr) )
    except jdb.JdbException, inst:
        return "E00"
    return "OK"

def zPacket(s):
    "Clear breakpoint packet"
    bpType, addr, length = splitBreakpointPacket(s)

    try:
        jdb.clearBreakpoint( gdbmips.convertAddressToLine(addr) )
    except jdb.JdbException, inst:
        return "E00"
    return "OK"

def breakpointPackage():
    # %02x:%08x" % (gdbmips.R_PC, next_address)
    return "T05%02x:%08x;" % (gdbmips.R_PC, unsigned(jdb.readRegister(gdbmips.R_PC)))

def cPacket(s):
    "Continue packet"
    addr = 0
    if s != "":
        addr = int(s,16)
    try:
        next_address = jdb.doContinue(addr)
    except jdb.JdbException, inst:
        return "X00"
    return breakpointPackage()

def vPacket(s):
    "v packet (multi-letter names)"
    if s == "Cont?":
        return "vCont;c"
    split = s.split(";")
    if split[0] == "Cont":
        if len(split) > 1 and split[1] == "c":
            next_address = jdb.doContinue()
            return breakpointPackage()
    return ""

def kPacket(s):
    "Kill packet"
    jdb.kill()
    raise PacketException("Gdb exits")


def handle(s):
    "Handle incoming packets. Returns a string with the outgoing packet"
    handlers = { "p" : pPacket, # read register packet
                 "P" : PPacket, # Write register packet
                 "q" : qPacket, # Query packet
                 "c" : cPacket, # Continue packet
                 "g" : gPacket, # Read all registers
                 "k" : kPacket, # Kill request (gdb exits)
                 "m" : mPacket, # Read memory
                 "M" : MPacket, # Write memory
                 "v" : vPacket, # Multi-letter names
                 "Z" : ZPacket, # Set breakpoints
                 "z" : zPacket, # Clear breakpoints
                 "?" : haltPacket,
                 }

    if handlers.has_key(s[0]):
        return handlers[s[0]](s[1:])

    return EMPTY

inputQueue = Queue(0)
killed = False

def enqueue(item):
    print "enq", item
    inputQueue.put(item)

def run(gdb):
	while True:
		try:
			packet = inputQueue.get(timeout=1)
		except Empty:
			continue
		response = handle(packet)
		if response == None:
			# Some error - return
			gdb.nak()
		else:
			gdb.writePacket(response)

