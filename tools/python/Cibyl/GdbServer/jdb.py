######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      jdb.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   jdb frontend for the Cibyl GDB server
##
## $Id:$
##
######################################################################
import re
import gdbmips
from Cibyl.BinaryTranslation.Mips import mips

backtraceRegexp = re.compile("[ \t]*\[([0-9]*)\] ([a-z,A-Z,_,0-9]*).([a-z,A-Z,_,0-9]*) \(([a-z,A-Z,0-9,_,.]*):([0-9]*)\)")

process = None
memoryArraySize = 0
stateValid = False
regs = {}

class JdbException(Exception):
    pass

def readRegister(reg):
    global stateValid
    if stateValid == False:
        readState()
    if reg == 0:
        return 0
    if reg in (gdbmips.R_HI, gdbmips.R_LO, gdbmips.R_K0, gdbmips.R_RA ):
        return readStaticVariable("CRunTime.%s" % gdbmips.registerNames[reg])
    return regs[reg]

def readStaticVariable(var):
    kickJdb()
    sendCommand("print %s" % var)
    output = waitForPrompt()

    for line in output.splitlines():
        if line.find(" = ") != -1:
            s = line.split()
            return int(s[-1])
    raise JdbException("Variable %x is not possible to read" % var)

def readMemory(addr, length):
    out = []
    unaligned = addr & 3
    unaligned_end = (addr+length) & 3
    val = 0

    if addr / 4 < 0 or addr / 4 >= memoryArraySize:
        raise JdbException("Address outside of memory")

    if unaligned != 0:
        val = readJdbMemoryVector(addr / 4)
    for start in range(addr, addr + unaligned):
        b = 3 - (start & 3)
        out.append( (val >> (b*8)) & 0xff )
    for start in range(addr+unaligned, addr + length - unaligned_end, 4):
        val = readJdbMemoryVector(start / 4)
        out.append((val & 0xff000000) >> 24)
        out.append((val & 0x00ff0000) >> 16)
        out.append((val & 0x0000ff00) >>  8)
        out.append((val & 0x000000ff))
    if unaligned_end != 0:
        val = readJdbMemoryVector((addr + length - unaligned_end) / 4)
    for start in range(addr + length - unaligned_end, addr + length + unaligned_end):
        b = 3 - (start & 3)
        out.append( (val >> (b*8)) & 0xff )
    return out

def writeMemory(address, length, value):
    if length != 4:
        print "Writing non-4-byte values not supported (%d bytes, address %x, val %s)" % (length, address, value)
        raise JdbException("Writing non-4-byte values not supported (%d bytes, address %x, val %s)" % (length, address, value))

    if address / 4 < 0 or address / 4 >= memoryArraySize:
        raise JdbException("Address outside of memory")

    value = int(value, 16)
    kickJdb()
    offset = address / 4
    sendCommand("set CRunTime.memory[%d] = %d" % (offset, value) )
    output = waitForPrompt()
    if output.find("CRunTime.memory[%d] = %d = %d" % (offset, value, value)) == -1:
        raise JdbException("Cannot write memory at %d" % offset)

def singleStep(addr):
    global stateValid
    stateValid = False
    return addr+4

def setBreakpoint(line):
    kickJdb()
    sendCommand("stop at CompiledProgram:%d" % line)
    output = waitForPrompt()
    if output.find("Unable to set") != -1:
        raise JdbException("Cannot set breakpoint at line %d" % line)

def clearBreakpoint(line):
    kickJdb()
    sendCommand("clear CompiledProgram:%d" % line)
    output = waitForPrompt()
    if output.find("Not found: breakpoint") != -1:
        raise JdbException("Cannot clear breakpoint at line %d" % line)

def doContinue(addr):
    global stateValid
    stateValid = False
    kickJdb()
    sendCommand("resume")
    output = waitForPrompt()
    return 0

def readJdbMemoryVector(offset):
    kickJdb()
    sendCommand("print CRunTime.memory[%d]" % offset)
    output = waitForPrompt()

    for line in output.splitlines():
        if line.find("] = ") != -1:
            s = line.split()
            return int(s[-1])
    raise JdbException("Memory offset %x not possible to read" % offset)

def readState():
    global stateValid
    kickJdb()
    process.sendline("locals")

    # Assume all registers are invalid
    for k in range(1, gdbmips.N_REGS):
        regs[k] = 0x000c1b41

    # Get the register context
    output = waitForPrompt()
    for line in output.splitlines():
        if line.find(" = ") != -1:
            s = line.split()
            if mips.namesToRegisters.has_key(s[0]):
                regs[mips.namesToRegisters[s[0]]] = int(s[2])

    # Get the PC
    kickJdb()
    process.sendline("where")
    output = waitForPrompt()
    for line in output.splitlines():
        match = backtraceRegexp.match(line)
        if match:
            level = int(match.group(1))
            fileName = match.group(4)
            lineNr = int(match.group(5))
            if level == 1 and fileName == "CompiledProgram.j":
                regs[gdbmips.R_PC] = gdbmips.convertLineToAddress(lineNr)

    stateValid = True

def getMemoryArraySize():
    global memoryArraySize
    kickJdb()
    sendCommand("print CRunTime.memory")
    output = waitForPrompt()
    for line in output.splitlines():
        if line.find("CRunTime.memory = instance of int[") != -1:
            memoryArraySize = int(line[ line.find("[") + 1 : line.find("]") ])
            return
    raise JdbException("Cannot get memory size")

def kickJdb():
    process.sendline("")
    waitForPrompt()

def sendCommand(s):
    process.sendline(s)

def waitForPrompt(prompt = "([a-z,A-Z,0-9,_])\[([0-9]+)\] $"):
    while True:
        try:
            process.expect(prompt, 2)
            return process.before
        except:
            sendCommand("\n")

def kill():
    process.close(True)

def init():
    waitForPrompt("> ")
    sendCommand("stop at CompiledProgram:0")
    waitForPrompt("> ")
    sendCommand("run")
    getMemoryArraySize()
    readState()
