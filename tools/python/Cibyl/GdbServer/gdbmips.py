######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      gdbmips.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   GDB-specific MIPS stuff
##
## $Id:$
##
######################################################################
import struct

R_K0 = 26
R_RA = 31

R_LO = 33
R_HI = 34
R_BAD= 35
R_CAUSE=36
R_PC = 37

R_FSR = 38+32
R_FIR = 38+32+1
R_FP  = 38+32+2

N_REGS = 38+32+2

registerNames = {
    R_RA : "ra",
}

elf = None # Elf object
textSection = None

def convertLineToAddress(lineNr):
    return lineNr * 4 + elf.getEntryPoint()

def convertAddressToLine(address):
    return (address - elf.getEntryPoint()) / 4

def readMemory(address, length):
    if address & 3 != 0:
        print "Unaligned reads in the text segment are unhandled"
        return
    out = []
    for b in range(address, address + length):
        cur = struct.unpack("B", textSection[b - elf.getEntryPoint()])
        out.append(cur[0])
    # if this is a Java invocation - just return a NOP
    if (out[0] == 0xfe and out[1] == 0xfe) or out[0] == 0xff:
        return [0,0,0,0] # NOP
    return out
