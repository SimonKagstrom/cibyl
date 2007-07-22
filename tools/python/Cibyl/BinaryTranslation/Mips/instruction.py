######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:      instruction.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Describes a MIPS instruction
##
## $Id: instruction.py 13921 2007-03-02 08:36:42Z ska $
##
######################################################################
import sys
from sets import Set

from Cibyl.BinaryTranslation import register, bytecode
import mips
from Cibyl.BinaryTranslation.Mips.optimizer import KnownValue, UnknownValue
from Cibyl.BinaryTranslation.Optimization.Builtins import builtins
from Cibyl.BinaryTranslation.labelmanager import Label

from Cibyl import config

def newInstruction(opCode):
    try:
	return insnToJavaInstruction[opCode][0]
    except:
	return Unimplemented

class Instruction(bytecode.ByteCodeGenerator, register.RegisterHandler):
    def __init__(self, controller, address, format, opCode, rd, rs, rt, extra):
	bytecode.ByteCodeGenerator.__init__(self, controller)
	register.RegisterHandler.__init__(self, controller, self)
	self.address = address
	self.opCode = opCode
	self.rs = rs
	self.rt = rt
	self.rd = rd
	self.extra = extra
	self.format = format
	self.controller = controller
	self.optimizer = controller.getOptimizer()
	self.delayed = None
	self.prefix = None # For lw/sw flushes
	self.isBranch = False
	self.isBranchDestination = False
	self.isFunctionCall = False
	self.isReturnInstruction = False
	# Destination register and source registers
	self.destinations = Set()
	self.sources = Set()

    def setBasicBlock(self, bb):
        self.basicBlock = bb
        if self.delayed:
            self.delayed.setBasicBlock(bb)
        if self.prefix:
            self.prefix.setBasicBlock(bb)

    def getBasicBlock(self):
        return self.basicBlock

    def addDelayedInstruction(self, insn):
	"Add an instruction in the delay-slot of this instruction"
	self.delayed = insn

    def pushLabel(self, address):
	label = self.controller.getLabel(address)
	self.pushConst(label.address)

    # Generic vector-handling code
    def pushVector(self, name, address):
	"Generic code to push the address to a vector"
 	self.pushStatic("CRunTime/%s [I" % (name))
	self.pushConst(address)

    # Memory-related functions
    def pushMemoryAddress(self, reg, imm):
	# Normal memory access
	self.pushRegister( reg )

	if imm != 0:
	    self.pushConst( imm )
	    self.emit("iadd")


    def pushMemoryIndex(self, reg, imm):
	"Push a memory address onto the stack"
	if reg in mips.memoryAddressRegisters:
	    # The special register contains the address >> 2
	    imm = imm / 4
	    self.pushRegister(reg)
	    if imm != 0:
		self.pushConst( imm )
		self.emit("iadd")
	else:
	    # Normal memory access
	    self.pushRegister( reg )

	    self.pushConst( 2 )
	    self.emit("iushr") # (reg + imm) / 4
	    if imm != 0:
		self.pushConst( imm / 4 )
		self.emit("iadd")

    def fixup(self):
	pass

    def setJavaMethod(self, method):
	"Define which java method this instruction is in"
	self.javaMethod = method

    def getJavaMethod(self):
	"Return which java method this instruction is in"
	return self.javaMethod

    def setFunction(self, function):
        self.function = function

    def getFunction(self):
        return self.function

    def getDestinationRegisters(self):
	out = Set(self.destinations)
	if self.delayed:
	    out = out.union(self.delayed.destinations)
	if self.prefix:
	    out = out.union(self.prefix.destinations)
	return out

    def getSourceRegisters(self):
	out = Set(self.sources)
	if self.delayed:
	    out = out.union(self.delayed.sources)
	if self.prefix:
	    out = out.union(self.prefix.sources)
	return out

    def trace(self):
	"Trace this instruction."
	self.ldc(str(self))
	self.invokestatic("CRunTime/emitTrace(Ljava/lang/String;)V")

    def nullify(self):
	"Deactivate the compilation of this instruction"
	self.sources = Set()
	self.destinations = Set()
	self.compile = self.nullifiedCompile

    def isDisabled(self):
	return self.compile == self.nullifiedCompile

    def nullifiedCompile(self):
	if self.delayed:
	    self.delayed.compile()

    def compile(self):
	"Compile this instruction into Java assembly"
	pass

    def run(self):
	"Run the instruction, returns True if the destination register is valid after the instruction"
	return False

    def maxOperandStackHeight(self):
        return 2

    def emit(self, what):
	if what.startswith("L_"):
	    self.controller.emit("%s" % what)
	else:
	    self.controller.emit("\t%s" % what)

    def __str__(self):
	return "0x%08x: %10s - rd %s, rs %s, rt %s, ext 0x%08x" % (self.address, mips.opStrings[self.opCode],
								   mips.registerNames[self.rd],
								   mips.registerNames[self.rs],
								   mips.registerNames[self.rt],
								   self.extra)

# Generic instructions
class Ifmt(Instruction):
    """This handles most of the I-format instructions"""
    def compile(self):
	signExtend = insnToJavaInstruction[self.opCode][2]
	byteCodeOp = insnToJavaInstruction[self.opCode][1]

	self.pushRegister( self.rs )
	if signExtend:
	    self.pushConst( self.extra & 0xffff )
	else:
	    self.pushConst( self.extra )
	self.emit( byteCodeOp )
	self.popToRegister( self.rt )

    def fixup(self):
	self.destinations = Set([self.rt])
	self.sources = Set([self.rs])

    def run(self):
	"Execute this instruction"
	op = insnToJavaInstruction[self.opCode][3]
	dst = self.optimizer.getRegisterValue(self.rs).doOperation(op, KnownValue(self.extra))
	self.optimizer.setRegisterValue(self.rt, dst)
	return isinstance(dst, KnownValue)

    def __str__(self):
	return "0x%08x: %10s %s, %s, 0x%08x" % (self.address, mips.opStrings[self.opCode],
						mips.registerNames[self.rt],
						mips.registerNames[self.rs],
						self.extra)

class Rfmt(Instruction):
    """This class handles most of the R-format instructions"""
    def compile(self):
	self.pushRegister( self.rs )
	self.pushRegister( self.rt )
	self.emit( insnToJavaInstruction[self.opCode][1] )
	self.popToRegister( self.rd )

    def fixup(self):
	self.destinations = Set([self.rd])
	self.sources = Set([self.rs, self.rt])

    def run(self):
	op = insnToJavaInstruction[self.opCode][2]
	dst = self.optimizer.getRegisterValue(self.rt).doOperation(op, self.optimizer.getRegisterValue(self.rs))
	self.optimizer.setRegisterValue(self.rd, dst)
	return isinstance(dst, KnownValue)
    def __str__(self):
	return "0x%08x: %10s %s, %s, %s" % (self.address, mips.opStrings[self.opCode],
					    mips.registerNames[self.rd],
					    mips.registerNames[self.rs],
					    mips.registerNames[self.rt])

class Addu(Rfmt):
    """Handle addu specially to handle move"""
    def compile(self):
        if self.rt == 0 and self.rs == self.rd:
            # Catch move v0, v0 and similar
            if config.debug:
                self.emit("nop")
	elif self.rt == 0:
	    self.pushRegister(self.rs)
	    self.popToRegister(self.rd)
	elif self.rs == 0:
	    self.pushRegister(self.rt)
	    self.popToRegister(self.rd)
	else:
	    Rfmt.compile(self)

class Subu(Rfmt):
    """Handle subu specially to handle negations"""
    def compile(self):
	if self.rs == 0:
	    self.pushRegister(self.rt)
	    self.emit("ineg")
	    self.popToRegister(self.rd)
	else:
	    Rfmt.compile(self)

class Multxx(Instruction):
    """mult etc, which are implemented in CRunTime.java"""
    def compile(self):
	function = insnToJavaInstruction[self.opCode][1]
	self.pushRegister( self.rs )
	self.pushRegister( self.rt )
	self.invokestatic("CRunTime/%s(II)J" % function)
        self.dup2()
        self.pushConst(32)
        self.lushr()
        self.l2i()
        self.popToRegister( mips.R_HI)
        self.l2i()
        self.popToRegister( mips.R_LO)

    def compileLowWordResult(self):
	"Optimized version of mult/div to produce 32-bit results only"
	op = insnToJavaInstruction[self.opCode][3]
	self.pushRegister( self.rs )
	self.pushRegister( self.rt )
	self.emit(op)
	self.popToRegister( mips.R_LO )

    def compileHighWordResult(self):
	"Optimized version of mult/div to produce 32-bit results only"
	op = insnToJavaInstruction[self.opCode][4]
	self.pushRegister( self.rs )
	self.pushRegister( self.rt )
	self.emit(op)
	self.popToRegister( mips.R_HI )

    def maxOperandStackHeight(self):
        return 5

    def fixup(self):
	self.destinations = Set([ mips.R_HI, mips.R_LO ])
	self.sources = Set([self.rs, self.rt])

    def run(self):
	rt = self.optimizer.getRegisterValue(self.rt)
	rs = self.optimizer.getRegisterValue(self.rt)
	res = rt * rs
	hi = (res >> KnownValue(32)) & KnownValue(0xffffffffl)
	lo = res & KnownValue(0xffffffffl)
	self.optimizer.setRegisterValue(mips.R_LO, lo)
	self.optimizer.setRegisterValue(mips.R_HI, hi)
	return False # FIXME!

class Mult(Multxx):
    def compile(self):
	self.pushRegister( self.rs )
        self.i2l()
	self.pushRegister( self.rt )
        self.i2l()
	self.lmul()
        self.dup2()
        self.pushConst(32)
        self.lushr()
        self.l2i()
        self.popToRegister( mips.R_HI)
        self.l2i()
        self.popToRegister( mips.R_LO)

class Div(Multxx):
    def compile(self):
	self.pushRegister( self.rs )
        self.i2l()
	self.pushRegister( self.rt )
        self.i2l()
	self.ldiv()
	self.pushRegister( self.rs )
        self.i2l()
	self.pushRegister( self.rt )
        self.i2l()
	self.lrem()
        self.l2i()
        self.popToRegister( mips.R_HI)
        self.l2i()
        self.popToRegister( mips.R_LO)

# Special-cases
class ShiftInstructions(Rfmt):
    """The shifts are a special case of the R-format."""
    def compile(self):
	if self.rt == 0 and self.extra == 0 and self.rd == 0:
	    # This is a nop
	    if config.debug:
		self.nop()
	    return
	self.pushRegister( self.rt )
	self.pushConst( self.extra )
	self.emit( insnToJavaInstruction[self.opCode][1] )
	self.popToRegister( self.rd )

    def maxOperandStackHeight(self):
	if self.rt == 0 and self.extra == 0 and self.rd == 0:
            return 0
        return 2

    def __str__(self):
	return "0x%08x: %10s %s, %s, 0x%08x" % (self.address, mips.opStrings[self.opCode],
						mips.registerNames[self.rd],
						mips.registerNames[self.rt],
						self.extra)

class ShiftInstructionsV(Rfmt):
    """sllv, srlv and friends"""
    def compile(self):
	self.pushRegister( self.rt ) # The order is opposite here!
	self.pushRegister( self.rs )
	self.emit( insnToJavaInstruction[self.opCode][1] )
	self.popToRegister( self.rd )

class Addi(Ifmt):
    """addiu instruction, catches assignments"""
    def compile(self):
	if self.rs == 0:
	    self.pushConst(self.extra)
	    self.popToRegister(self.rt)
	elif self.rt == self.rs and not register.regIsStatic(self.rt):
	    self.iinc(self.rt, self.extra)
	else:
	    Ifmt.compile(self)

class Nor(Rfmt):
    """nor instruction, twostep"""
    def compile(self):
	self.pushRegister( self.rs )
	self.pushRegister( self.rt )
	self.emit("ior")
	self.pushConst(-1)
	self.emit("ixor")
	self.popToRegister( self.rd )

    def run(self):
	dst = (self.optimizer.getRegisterValue(self.rt) | self.optimizer.getRegisterValue(self.rs)) ^ (-1)
	self.optimizer.setRegisterValue(self.rd, dst)
	return isinstance(dst, KnownValue)

class Lui(Ifmt):
    """lui instruction"""
    def compile(self):
	self.pushConst(self.extra << 16)
	self.popToRegister(self.rt)

    def run(self):
	self.optimizer.setRegisterValue(self.rt, KnownValue(self.extra << 16))

    def maxOperandStackHeight(self):
        return 1

    def __str__(self):
	return "0x%08x: %10s %s, 0x%08x" % (self.address, mips.opStrings[self.opCode],
					    mips.registerNames[self.rt],
					    self.extra)


class MemoryAccess(Instruction):
    def __str__(self):
	out = "0x%08x: %10s %s, 0x%04x(%s)" % (self.address, mips.opStrings[self.opCode],
					       mips.registerNames[self.rt],
					       self.extra,
					       mips.registerNames[self.rs])
	if self.delayed:
	    out += "\\n  `- " + str(self.delayed)
	return out

    def maxOperandStackHeight(self):
        return 4


class LoadXX(MemoryAccess):
    """Load a byte or short from memory."""
    def compile(self):
	self.pushRegister( self.rs )
	if self.extra != 0:
	    self.pushConst( self.extra )
	    self.emit("iadd")
	self.invokestatic("CRunTime/memoryRead%s(I)I" % insnToJavaInstruction[self.opCode][1])
	self.popToRegister( self.rt )
    def fixup(self):
	self.destinations = Set([self.rt])
	self.sources = Set([self.rs, self.rt])
    def run(self):
	self.optimizer.setRegisterValue(self.rt, UnknownValue())
	return False

class StoreXX(MemoryAccess):
    """Store a byte or short to memory."""
    def compile(self):
	self.pushMemoryAddress(self.rs, self.extra)
	self.pushRegister( self.rt )
	self.invokestatic("CRunTime/memoryWrite%s(II)V" % insnToJavaInstruction[self.opCode][1])
    def fixup(self):
	self.sources = Set([ self.rs, self.rt ])
    def run(self):
	return False

class Lbu(LoadXX):
    """Read a word from memory."""
    def compile(self):
	if self.rt == mips.R_RA:
	    if config.verbose: print "Skipping lb to RA", self
	    return None
	if self.prefix:
	    self.prefix.compile()
	self.getstatic("CRunTime/memory [I")

	# val = CRunTime.memory[ address / 4 ]
	self.pushMemoryIndex(self.rs, self.extra)
	self.emit("iaload")

	# b = 3 - (address & 3)
	self.pushConst(3)
	self.pushMemoryAddress(self.rs, self.extra)
	self.pushConst(3)
	self.emit("iand")
	self.emit("isub")

	# b = b * 8
	self.pushConst(3)
	self.emit("ishl")

	# out = val >> (b*8)
	self.emit("ishr")
	self.pushConst(0xff)
	self.emit("iand")

	self.popToRegister( self.rt )

class Lw(LoadXX):
    """Read a word from memory."""
    def compile(self):
	if self.rt == mips.R_RA:
	    if not config.debug:
		if config.verbose: print "Skipping lw to RA", self
		return None
	if self.prefix:
	    self.prefix.compile()
	self.getstatic("CRunTime/memory [I")
	self.pushMemoryIndex(self.rs, self.extra)
	self.emit("iaload")
	self.popToRegister( self.rt )

class Sw(StoreXX):
    """Store a word to memory."""
    def compile(self):
	if self.rt == mips.R_RA:
	    if not config.debug:
		if config.verbose: print "Skipping sw of RA", self
		return None
	if self.prefix:
	    self.prefix.compile()
	self.getstatic("CRunTime/memory [I")
	self.pushMemoryIndex(self.rs, self.extra)
	self.pushRegister( self.rt )
	self.emit("iastore")


class Mfxx(Instruction):
    """mflo and mfhi"""
    def compile(self):
	self.pushRegister( insnToJavaInstruction[self.opCode][2] )
	self.popToRegister( self.rd )

    def fixup(self):
	self.destinations = Set([ self.rd ])
	self.sources = Set([ insnToJavaInstruction[self.opCode][2] ])

    def run(self):
	src = self.optimizer.getRegisterValue(insnToJavaInstruction[self.opCode][2])
	self.optimizer.setRegisterValue(self.rd, src)
	return isinstance(src, KnownValue)

    def maxOperandStackHeight(self):
        return 1



class Mtxx(Instruction):
    """mtlo and mthi"""
    def compile(self):
	self.pushRegister( self.rs )
	self.popToRegister( insnToJavaInstruction[self.opCode][2] )
    def fixup(self):
	self.destinations = Set([ insnToJavaInstruction[self.opCode][2] ])
	self.sources = Set([ self.rs ])
    def run(self):
	src = self.rs
	dst = insnToJavaInstruction[self.opCode][2]
	self.optimizer.setRegisterValue(dst, src)
	return isinstance(dst, KnownValue)

    def maxOperandStackHeight(self):
        return 1


class Syscall(Instruction):
    """The special syscall encoding"""
    def compile(self):
	call = self.controller.hasSyscall(self.extra) # Will throw an exception if this is not present
	name = call.getName()
	args = "I" * call.getNrArgs()
	ret = "V"
	if call.getReturnType() != "void":
	    ret = "I"
	self.invokestatic("Syscalls/%s(%s)%s" % (name, args, ret) )
	if call.getReturnType() != "void":
	    self.popToRegister( mips.R_V0 )
    def fixup(self):
	call = self.controller.hasSyscall(self.extra) # Will throw an exception if this is not present
	if call.getReturnType() != "void":
	    self.destinations = Set([ mips.R_V0 ])
    def __str__(self):
	call = self.controller.hasSyscall(self.extra)
	return "0x%08x: syscall %s (%d)" % (self.address, call, self.extra)
    def run(self):
	call = self.controller.hasSyscall(self.extra)
	if call.getReturnType() != "void":
	    self.optimizer.setRegisterValue(mips.R_V0, UnknownValue())
	return False

    def maxOperandStackHeight(self):
	call = self.controller.hasSyscall(self.extra) # Will throw an exception if this is not present
        return call.getNrArgs() + 1


class SyscallRegisterArgument(Instruction):
    """The special encoding for register syscall arguemnts"""
    def compile(self):
	self.pushRegister( self.extra )
    def fixup(self):
	self.sources = Set([ self.extra ])
    def __str__(self):
	return "0x%08x: syscall_push_r %s" % (self.address, mips.registerNames[ self.extra ])

    def maxOperandStackHeight(self):
        return 1

    def run(self):
	return False

class AssignMemoryRegister(Instruction):
    """Special generated instruction to make memory accesses more efficient"""
    def __init__(self, controller, address, format, opCode, rd, rs, rt, extra):
	Instruction.__init__(self, controller, address, format, opCode, rd, rs, rt, extra)
	self.sources = Set([ self.rs ])
	self.destinations = Set([ self.rd ])

    def compile(self):
	self.controller.registerHandler.resetMemoryRegister(self.rd)
	# Does rd = rs >> 2
	self.pushRegister(self.rs)
	self.pushConst(2)
	self.emit("iushr")
	self.popToRegister(self.rd)

    def __str__(self):
	return "  %s = %s >> 2" % (mips.registerNames[ self.rd ], mips.registerNames[ self.rs ])

class BranchInstruction(Instruction):
    def run(self):
	return False

class Jump(BranchInstruction):
    """j instruction"""
    def __init__(self, controller, address, format, opCode, rd, rs, rt, extra):
	Instruction.__init__(self, controller, address, format, opCode, rd, rs, rt, extra)

    def compile(self):
	if self.delayed:
	    self.delayed.compile()
	ownMethod = self.getJavaMethod()
	labMethod = self.controller.getLabel(self.extra << 2).getJavaMethod()
        assert(ownMethod == labMethod)
        self.goto(self.extra << 2)


    def fixup(self):
	self.controller.addLabel(self.extra << 2)
	self.isBranch = True

    def maxOperandStackHeight(self):
        return 0

    def __str__(self):
	out = "0x%08x: %10s 0x%08x" % (self.address, mips.opStrings[self.opCode], self.extra << 2)
    	if self.delayed:
	    out += "\\n  `- " + str(self.delayed)
	return out


class Jal(BranchInstruction):
    """jal instruction."""
    def __init__(self, controller, address, format, opCode, rd, rs, rt, extra):
	Instruction.__init__(self, controller, address, format, opCode, rd, rs, rt, extra)
	self.isFunctionCall = True

	# Handle BAL instructions (which are PC-relative)
	if self.opCode == mips.OP_BGEZAL:
	    self.dstAddress = self.address + (self.extra << 2) + 4
	else:
	    self.dstAddress = self.extra << 2

    def compile(self):
	if self.delayed:
	    self.delayed.compile()
	otherMethod = self.controller.lookupJavaMethod(self.dstAddress)

        if self.builtin and otherMethod.name in builtins.alwaysInline or self.getFunction().name in config.inlineBuiltinsFunctions:
            return self.builtin.compile()

	for reg in otherMethod.getRegistersToPass():
	    self.pushRegister(reg)
	# Write RA if debug is enabled
	if config.debug:
	    self.pushConst(self.address + 8) # Skip the delayed instruction
	    self.popToRegister(mips.R_RA)

        otherMethod.invoke(self.dstAddress)
	if otherMethod.clobbersReg(mips.R_V1):
	    self.getstatic("CRunTime/saved_v1 I")
	    self.popToRegister(mips.R_V1)

	if otherMethod.getJavaReturnType() == "I":
	    self.popToRegister(mips.R_V0)

    def compileJump(self):
        "Compile for the case where the destination function is in the same java method"
	if self.delayed:
	    self.delayed.compile()
        self.goto(self.dstAddress)

    def fixup(self):
	self.controller.addLabel(self.dstAddress, inJumpTab = True)
	self.isBranch = True
	self.destinations = Set([ mips.R_RA ])

        otherName = self.controller.elf.getSymbolByAddress(self.dstAddress).name

        # Is this something we should inline?
        self.builtin = builtins.match(self.controller, self, otherName)

    def maxOperandStackHeight(self):
	otherMethod = self.controller.lookupJavaMethod(self.dstAddress)
        if self.builtin and otherMethod.name in builtins.alwaysInline or self.getFunction().name in config.inlineBuiltinsFunctions:
            return self.builtin.maxOperandStackHeight()
        return len(otherMethod.getRegistersToPass()) + 2

    def __str__(self):
	out = "0x%08x: %10s 0x%08x" % (self.address, mips.opStrings[self.opCode], self.dstAddress)
    	if self.delayed:
	    out += "\\n  `- " + str(self.delayed)
	return out

class Jalr(BranchInstruction):
    """jalr instruction."""
    def __init__(self, controller, address, format, opCode, rd, rs, rt, extra):
	Instruction.__init__(self, controller, address, format, opCode, rd, rs, rt, extra)
	self.isFunctionCall = True

    def compile(self):
	"Compile for 1-1 Java method, C functions"
	self.pushRegister(self.rs)
	if self.delayed:
	    self.delayed.compile()
	self.pushRegister(mips.R_SP)
	self.pushRegister(mips.R_A0)
	self.pushRegister(mips.R_A1)
	self.pushRegister(mips.R_A2)
	self.pushRegister(mips.R_A3)
	if config.debug:
	    self.pushConst(self.address + 8) # Skip the delayed instruction
	    self.popToRegister(mips.R_RA)
	self.invokestatic("CompiledProgram/__CIBYL_global_jumptab(IIIIII)I")
	self.getstatic("CRunTime/saved_v1 I")
	self.popToRegister(mips.R_V1)
	self.popToRegister(mips.R_V0)

    def fixup(self):
	self.isBranch = True
	self.sources = Set([ self.rs ])
	self.destinations = Set([ self.rd ])

    def maxOperandStackHeight(self):
        return 9

    def __str__(self):
	out = "0x%08x: %10s %s, %s" % (self.address, mips.opStrings[self.opCode],
				       mips.registerNames[ self.rd ], mips.registerNames[ self.rs ])
    	if self.delayed:
	    out += "\\n  `- " + str(self.delayed)
	return out

class Jr(BranchInstruction):
    """Jump to a register destination."""
    def compile(self):
	if self.rs == mips.R_RA:
	    # Mark this as a return instruction
	    self.isReturnInstruction = True

	    if self.delayed:
		self.delayed.compile()
	    self.goto("__CIBYL_function_return")
	else:
	    # Push the local register and go to the local jumptab
	    self.pushRegister(self.rs)
	    if self.delayed:
		self.delayed.compile()
	    self.goto("__CIBYL_local_jumptab")

    def fixup(self):
	self.isBranch = True
	self.sources = Set([ self.rs ])

    def maxOperandStackHeight(self):
        return 1

    def __str__(self):
	out = "0x%08x: %10s %s" % (self.address, mips.opStrings[self.opCode],
				   mips.registerNames[self.rs])
    	if self.delayed:
	    out += "\\n  `- " + str(self.delayed)
	return out

class SetInstruction(Instruction):
    """Convenience class to handle slt, slti and friends"""
    pass

class OneRegisterSetInstruction(SetInstruction):
    """slti and friends, itype"""
    def compile(self):
	self.pushRegister( self.rs )
	self.pushConst( self.extra )
	self.invokestatic("CRunTime/%s(II)I" % insnToJavaInstruction[self.opCode][1])
	self.popToRegister(self.rt)

    def fixup(self):
	self.sources = Set([ self.rs ])
	self.destinations = Set([ self.rt ])

    def __str__(self):
	return "0x%08x: %10s %s, %s, 0x%08x" % (self.address, mips.opStrings[self.opCode],
						mips.registerNames[self.rt],
						mips.registerNames[self.rs],
						self.extra)

class Slti(OneRegisterSetInstruction):
    "slti"
    def compile(self):
	self.pushRegister( self.rs )
	self.pushConst( self.extra )
	self.emit("isub")
	self.pushConst(31)
	self.emit("iushr")
	self.popToRegister(self.rt)

class TwoRegisterSetInstruction(SetInstruction):
    """slt and friends, rtype"""
    def compile(self):
	# Special case for comparison to zero
	if self.rs == mips.R_ZERO:
	    self.pushConst(1)
	    self.pushRegister(self.rt)
	    self.emit("ifne L_tmp_%x" % self.address)
	    self.pop()
	    self.pushConst(0)
	    self.emit("L_tmp_%x:" % self.address)
	else:
	    self.pushRegister( self.rs )
	    self.pushRegister( self.rt )
	    self.invokestatic("CRunTime/%s(II)I" % insnToJavaInstruction[self.opCode][1])
	self.popToRegister(self.rd)

    def fixup(self):
	self.sources = Set([ self.rs, self.rt ])
	self.destinations = Set([ self.rd ])

    def maxOperandStackHeight(self):
        return 3

    def __str__(self):
	return "0x%08x: %10s %s, %s, %s" % (self.address, mips.opStrings[self.opCode],
					    mips.registerNames[self.rd],
					    mips.registerNames[self.rs],
					    mips.registerNames[self.rt])

class Slt(TwoRegisterSetInstruction):
    "slt"
    def compile(self):
	self.pushRegister( self.rs )
	self.pushRegister( self.rt )
	self.emit("isub")
	self.pushConst(31)
	self.emit("iushr")
	self.popToRegister(self.rd)

class OneRegisterConditionalJump(BranchInstruction):
    """bgez and friends"""
    def compile(self):
	self.pushRegister( self.rs )
	if self.delayed:
	    self.delayed.compile()
	self.emit("%s %s" % (insnToJavaInstruction[self.opCode][1],
			     str(self.controller.getLabel(self.address + (self.extra<<2) + 4))) )

    def fixup(self):
	self.controller.addLabel( self.address + (self.extra << 2) + 4 )
	self.isBranch = True
	self.sources = Set([ self.rs ])

    def maxOperandStackHeight(self):
        return 1

    def __str__(self):
	out = "0x%08x: %10s %s, 0x%08x" % (self.address, mips.opStrings[self.opCode],
					   mips.registerNames[self.rs],
					   self.address + (self.extra << 2) + 4)
    	if self.delayed:
	    out += "\\n  `- " + str(self.delayed)
	return out

class TwoRegisterConditionalJump(BranchInstruction):
    """beq and friend(s)"""
    def compile(self):
	self.pushRegister( self.rs )
	self.pushRegister( self.rt )
	if self.delayed:
	    self.delayed.compile()
	self.emit("%s %s" % (insnToJavaInstruction[self.opCode][1],
			     str(self.controller.getLabel(self.address + (self.extra<<2) + 4))) )
    def fixup(self):
	self.controller.addLabel( self.address + (self.extra << 2) + 4 )
	self.isBranch = True
	self.sources = Set([ self.rs, self.rt ])

    def __str__(self):
	out = "0x%08x: %10s %s, %s, 0x%08x" % (self.address, mips.opStrings[self.opCode],
					       mips.registerNames[self.rs],
					       mips.registerNames[self.rt],
					       self.address + (self.extra << 2) + 4)
    	if self.delayed:
	    out += "\\n  `- " + str(self.delayed)
	return out


class Nop(Instruction):
    """e.g., break"""
    def run(self):
	return True

    def compile(self):
	if config.debug:
	    self.emit("nop")

    def maxOperandStackHeight(self):
        if config.debug:
            return 1
        return 0


class Unimplemented(Instruction):
    """This instruction class has not yet been implemented. Emit
    warning"""
    def compile(self):
	print "; WARNING, unimplemented instruction ", self
	self.trace()
    def run(self):
	return False

insnToJavaInstruction = {
    ## R-format instructions
    mips.OP_ADDU : (Addu, "iadd", "+"),
    mips.OP_ADD  : (Rfmt, "iadd", "+"),  # Not necessarily correct
    mips.OP_SUB  : (Rfmt, "isub", "-"),  # Not necessarily correct
    mips.OP_SUBU : (Subu, "isub", "-"),
    mips.OP_XOR  : (Rfmt, "ixor", "^"),
    mips.OP_AND  : (Rfmt, "iand", "&"),
    mips.OP_OR   : (Rfmt,  "ior", "|"),
    mips.OP_NOR  : (Nor,    None),
    mips.OP_SRAV : (ShiftInstructionsV, "ishr", ">>"),
    mips.OP_SLLV : (ShiftInstructionsV, "ishl", "<<"),
    mips.OP_SRLV : (ShiftInstructionsV, "iushr", ">>>"),
    mips.OP_SLL  : (ShiftInstructions, "ishl", "<<"),   # Catch nops
    mips.OP_SRA  : (ShiftInstructions, "ishr", ">>"),
    mips.OP_SRL  : (ShiftInstructions, "iushr", ">>>"),

    ## I-format instructions
    mips.OP_ADDI : (Addi, "iadd", False, "+"),
    mips.OP_ADDIU: (Addi, "iadd", False, "+"),
    mips.OP_XORI : (Ifmt, "ixor", False, "^"),
    mips.OP_ANDI : (Ifmt, "iand", True,  "&"),
    mips.OP_ORI  : (Ifmt, "ior",  True,  "|"),

    ## J-format instructions
    mips.OP_JAL  : (Jal,  None),
    mips.OP_JALR : (Jalr, None),
    mips.OP_J    : (Jump, None),
    mips.OP_BGEZAL : (Jal,  None), # This handles BAL instructions

    ## Memory handling
    mips.OP_LW   : (Lw,   None),
    mips.OP_SW   : (Sw,   None),
    mips.OP_LB   : (LoadXX, "Byte"),
#    mips.OP_LBU  : (Lbu, None),
    mips.OP_LBU  : (LoadXX, "ByteUnsigned"),
    mips.OP_LH   : (LoadXX, "Short"),
    mips.OP_LHU  : (LoadXX, "ShortUnsigned"),
    mips.OP_LWL  : (LoadXX, "WordLeft"),
    mips.OP_LWR  : (Nop, None),               # Everything is done in LWL
    mips.OP_SB   : (StoreXX, "Byte"),
    mips.OP_SH   : (StoreXX, "Short"),
    mips.OP_SWL  : (StoreXX, "WordLeft"),
    mips.OP_SWR  : (Nop, None),               # Everything is done in SWL

    ## Misc other instructions
    mips.OP_BREAK: (Nop, None),
    mips.OP_MULT : (Mult,  "mult", "*", "imul"),
    mips.OP_MULTU: (Multxx, "multu", "*"),
    mips.OP_DIV  : (Div,   "div", "/", "idiv", "irem"),
    mips.OP_DIVU : (Multxx,  "divu", "/"),
    mips.OP_MFLO : (Mfxx, None, mips.R_LO),
    mips.OP_MFHI : (Mfxx, None, mips.R_HI),
    mips.OP_MTLO : (Mtxx, None, mips.R_LO),
    mips.OP_MTHI : (Mtxx, None, mips.R_HI),
    mips.OP_JR   : (Jr,   None), # Actually of R-type, but special
    mips.OP_LUI  : (Lui,  None),

    # Conditional jumps and slt etc.
    mips.OP_SLTU : (TwoRegisterSetInstruction,  "sltu", "<"), # Actually of R-type, but special
    mips.OP_SLT  : (Slt, "", ""),
    mips.OP_SLTIU: (OneRegisterSetInstruction,  "sltu", "<"), # Actually of I-type, but special
    mips.OP_SLTI : (Slti, "", ""),

    mips.OP_BEQ  : (TwoRegisterConditionalJump, "if_icmpeq"),
    mips.OP_BNE  : (TwoRegisterConditionalJump, "if_icmpne"),

    mips.OP_BGEZ : (OneRegisterConditionalJump, "ifge"),
    mips.OP_BGTZ : (OneRegisterConditionalJump, "ifgt"),
    mips.OP_BLEZ : (OneRegisterConditionalJump, "ifle"),
    mips.OP_BLTZ : (OneRegisterConditionalJump, "iflt"),
}
