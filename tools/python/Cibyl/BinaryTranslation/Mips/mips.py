######################################################################
##
## Copyright (C) 2006,  Simon Kagstrom
##
## Filename:	  Mips.py
## Author:		Simon Kagstrom <ska@bth.se>
## Description:   MIPS defs
##
## $Id: mips.py 12821 2006-12-22 14:46:47Z ska $
##
######################################################################
OP_ADD	= 1
OP_ADDI   = 2
OP_ADDIU  = 3
OP_ADDU   = 4
OP_AND	  = 5
OP_ANDI   = 6
OP_BEQ	= 7
OP_BGEZ   = 8
OP_BGEZAL = 9
OP_BGTZ   = 10
OP_BLEZ   = 11
OP_BLTZ   = 12
OP_BLTZAL = 13
OP_BNE	= 14
OP_DIV	= 16
OP_DIVU   = 17
OP_J	  = 18
OP_JAL	= 19
OP_JALR   = 20
OP_JR	 = 21
OP_LB	 = 22
OP_LBU	= 23
OP_LH	 = 24
OP_LHU	= 25
OP_LUI	= 26
OP_LW	 = 27
OP_LWL	= 28
OP_LWR	= 29
OP_MFHI   = 31
OP_MFLO   = 32
OP_MTHI   = 34
OP_MTLO   = 35
OP_MULT   = 36
OP_MULTU  = 37
OP_NOR	= 38
OP_OR	 = 39
OP_ORI	= 40
OP_RFE	= 41
OP_SB	 = 42
OP_SH	 = 43
OP_SLL	= 44
OP_SLLV   = 45
OP_SLT	= 46
OP_SLTI   = 47
OP_SLTIU  = 48
OP_SLTU   = 49
OP_SRA	= 50
OP_SRAV   = 51
OP_SRL	= 52
OP_SRLV   = 53
OP_SUB	= 54
OP_SUBU   = 55
OP_SW	 = 56
OP_SWL	= 57
OP_SWR	= 58
OP_XOR	= 59
OP_XORI   = 60
OP_SYSCALL= 61
OP_UNIMP  = 62
OP_RES	= 63
OP_MFCP0  = 65
OP_MTCP0  = 66
OP_CFCP0  = 67
OP_CTCP0  = 68
OP_BCCP0  = 69
OP_TLBR   = 70
OP_TLBWI  = 71
OP_TLBWR  = 72
OP_TLBP   = 73
OP_COP1   = 75
OP_COP2   = 76
OP_COP3   = 77
OP_BREAK  = 78

registerNames = {
	 0 : "zero",
	 1 : "at",
	 2 : "v0",
	 3 : "v1",
	 4 : "a0",
	 5 : "a1",
	 6 : "a2",
	 7 : "a3",
	 8 : "t0",
	 9 : "t1",
	10 : "t2",
	11 : "t3",
	12 : "t4",
	13 : "t5",
	14 : "t6",
	15 : "t7",
	16 : "s0",
	17 : "s1",
	18 : "s2",
	19 : "s3",
	20 : "s4",
	21 : "s5",
	22 : "s6",
	23 : "s7",
	24 : "t8",
	25 : "t9",
	26 : "k0",
	27 : "k1",
	28 : "gp",
	29 : "sp",
	30 : "s8", # or fp
	31 : "ra",
	32 : "hi", # special
	33 : "lo",

	# Not part of the regular instruction set
	34 : "cpc", # Cibyl cross-method PC
	35 : "cm0", # Cibyl memory access registers
	36 : "cm1",
	37 : "cm2",
	38 : "cm3",
	39 : "cm4",
	40 : "cm5",
	41 : "cm6",
	42 : "cm7",
	43 : "cm8",
	44 : "ecb", # Exception callback address
	45 : "ear", # Exception argument
	46 : "fna", # Function address (for multiple java methods)
	47 : "mem", # A reference to the memory
}

# An inversion of the above
namesToRegisters = dict([ (v, k) for k, v in registerNames.iteritems() ])

memoryAddressRegisters = [
	namesToRegisters["cm0"],
	namesToRegisters["cm1"],
	namesToRegisters["cm2"],
	namesToRegisters["cm3"],
	namesToRegisters["cm4"],
	namesToRegisters["cm5"],
	namesToRegisters["cm6"],
	namesToRegisters["cm7"],
	namesToRegisters["cm8"],
]

callerSavedRegisters = [
	namesToRegisters["s0"],
	namesToRegisters["s1"],
	namesToRegisters["s2"],
	namesToRegisters["s3"],
	namesToRegisters["s4"],
	namesToRegisters["s5"],
	namesToRegisters["s6"],
	namesToRegisters["s7"],
	namesToRegisters["s8"],
]


R_ZERO = namesToRegisters["zero"]
R_V0 = namesToRegisters["v0"]
R_V1 = namesToRegisters["v1"]
R_A0 = namesToRegisters["a0"]
R_A1 = namesToRegisters["a1"]
R_A2 = namesToRegisters["a2"]
R_A3 = namesToRegisters["a3"]
R_SP = namesToRegisters["sp"]
R_RA = namesToRegisters["ra"]
R_HI = namesToRegisters["hi"]
R_LO = namesToRegisters["lo"]
R_K1 = namesToRegisters["k1"]
R_CPC= namesToRegisters["cpc"]
R_ECB= namesToRegisters["ecb"] # Exception callback address
R_EAR= namesToRegisters["ear"] # Exception argument
R_FNA= namesToRegisters["fna"] # Function address (for multiple functions / Java method)
R_MEM= namesToRegisters["mem"]

N_REGS = len(namesToRegisters)

ifmtZeroExtend = [
	OP_ANDI,
	OP_ORI,
	OP_XORI,
]

delaySlotInstructions = [
	# Conditional jumps
	OP_BEQ,
	OP_BGEZ,
	OP_BGEZAL,
	OP_BGTZ,
	OP_BLEZ,
	OP_BLTZ,
	OP_BLTZAL,
	OP_BNE,
	# Unconditional jumps
	OP_J,
	OP_JAL,
	OP_JALR,
	OP_JR,
	# Load delay slots
#	OP_LB,
#	OP_LBU,
#	OP_LH,
#	OP_LHU,
#	OP_LW,
#	OP_LWL,
#	OP_LWR,
]

#The table below is used to translate bits 31:26 of the instruction
#into a value suitable for the "opCode" field of a MemWord structure,
#or into a special value for further decoding.
SPECIAL  = 100
BCOND = 101
COP0 = 102
COP1 = 103
COP2 = 104
COP3 = 105
CIBYL_SYSCALL  = 999
CIBYL_REGISTER_ARGUMENT  = 1000
CIBYL_ASSIGN_MEMREG = 1001

IFMT = 1
JFMT = 2
RFMT = 3

OPCODE = 0
FORMAT = 1

opTable = [
	[SPECIAL, RFMT], [BCOND, IFMT], [OP_J, JFMT], [OP_JAL, JFMT],
	[OP_BEQ, IFMT], [OP_BNE, IFMT], [OP_BLEZ, IFMT], [OP_BGTZ, IFMT],
	[OP_ADDI, IFMT], [OP_ADDIU, IFMT], [OP_SLTI, IFMT], [OP_SLTIU, IFMT],
	[OP_ANDI, IFMT], [OP_ORI, IFMT], [OP_XORI, IFMT], [OP_LUI, IFMT],
	[COP0, RFMT], [COP1, IFMT], [COP2, IFMT], [COP3,IFMT],
	[OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT],
	[OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT],
	[OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT],
	[OP_LB, IFMT], [OP_LH, IFMT], [OP_LWL, IFMT], [OP_LW, IFMT],
	[OP_LBU, IFMT], [OP_LHU, IFMT], [OP_LWR, IFMT], [OP_RES, IFMT],
	[OP_SB, IFMT], [OP_SH, IFMT], [OP_SWL, IFMT], [OP_SW, IFMT],
	[OP_RES, IFMT], [OP_RES, IFMT], [OP_SWR, IFMT], [OP_RES, IFMT],
	[OP_UNIMP, IFMT], [OP_UNIMP, IFMT], [OP_UNIMP, IFMT], [OP_UNIMP,IFMT],
	[OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT],
	[OP_UNIMP, IFMT], [OP_UNIMP, IFMT], [OP_UNIMP, IFMT], [OP_UNIMP,IFMT],
	[OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT], [OP_RES, IFMT]
	]

specialTable = [
	OP_SLL, OP_RES, OP_SRL, OP_SRA, OP_SLLV, OP_RES, OP_SRLV, OP_SRAV,
	OP_JR, OP_JALR, OP_RES, OP_RES, OP_SYSCALL, OP_BREAK, OP_RES, OP_RES,
	OP_MFHI, OP_MTHI, OP_MFLO, OP_MTLO, OP_RES, OP_RES, OP_RES, OP_RES,
	OP_MULT, OP_MULTU, OP_DIV, OP_DIVU, OP_RES, OP_RES, OP_RES, OP_RES,
	OP_ADD, OP_ADDU, OP_SUB, OP_SUBU, OP_AND, OP_OR, OP_XOR, OP_NOR,
	OP_RES, OP_RES, OP_SLT, OP_SLTU, OP_RES, OP_RES, OP_RES, OP_RES,
	OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES,
	OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES
	]

opStrings = [
	"Shouldn't happen", "add  ", "addi ", "addiu", "addu ", "and  ",
	"andi ", "beq  ", "bgez ", "bgezal", "bgtz ", "blez ", "bltz ",
	"bltzal", "bne  ", "Shouldn't happen", "div  ", "divu ", "j	",
	"jal  ", "jalr ", "jr   ", "lb   ", "lbu  ", "lh   ", "lhu  ",
	"lui  ", "lw   ", "lwl  ", "lwr  ", "Shouldn't happen", "mfhi ",
	"mflo ", "shouldn't happen", "mthi ", "mtlo ", "mult ","multu",
	"nor  ", "or   ", "ori  ", "rfe  ", "sb   ", "sh   ", "sll  ",
	"sllv ","slt  ", "slti ", "sltiu", "sltu ", "sra  ", "srav ",
	"srl  ", "srlv ", "sub  ", "subu ", "sw   ", "swl  ", "swr  ",
	"xor  ", "xori ", "syscall", "Unimplemented", "Reserved",
	"Shouldn't happen", "mfcp0", "mtcp0", "cfcp0", "ctcp0", "bccp0",
	"tlbr ", "tlbwi", "tlbwr", "tlbp ", "Shouldn't happen", "??CP1",
	"??CP2", "??CP3", "break"
	]
