/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      mips.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   MIPS implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <mips.hh>

mips_op_entry_t mips_op_entries[] =
{
  {SPECIAL, RFMT}, {BCOND, IFMT},    {OP_J, JFMT},    {OP_JAL, JFMT},
  {OP_BEQ, IFMT},  {OP_BNE, IFMT},   {OP_BLEZ, IFMT}, {OP_BGTZ, IFMT},
  {OP_ADDI, IFMT}, {OP_ADDIU, IFMT}, {OP_SLTI, IFMT}, {OP_SLTIU, IFMT},
  {OP_ANDI, IFMT}, {OP_ORI, IFMT},   {OP_XORI, IFMT}, {OP_LUI, IFMT},
  {COP0, RFMT},    {COP1, IFMT},     {COP2, IFMT},    {COP3,IFMT},
  {OP_RES, IFMT},  {OP_RES, IFMT},   {OP_RES, IFMT},  {OP_RES, IFMT},
  {OP_RES, IFMT},  {OP_RES, IFMT},   {OP_RES, IFMT},  {OP_RES, IFMT},
  {OP_RES, IFMT},  {OP_RES, IFMT},   {OP_RES, IFMT},  {OP_RES, IFMT},
  {OP_LB, IFMT},   {OP_LH, IFMT},    {OP_LWL, IFMT},  {OP_LW, IFMT},
  {OP_LBU, IFMT},  {OP_LHU, IFMT},   {OP_LWR, IFMT},  {OP_RES, IFMT},
  {OP_SB, IFMT},   {OP_SH, IFMT},    {OP_SWL, IFMT},  {OP_SW, IFMT},
  {OP_RES, IFMT},  {OP_RES, IFMT},   {OP_SWR, IFMT},  {OP_RES, IFMT},
  {OP_UNIMP, IFMT},{OP_LWC1, IFMT}, {OP_UNIMP, IFMT},{OP_UNIMP,IFMT},
  {OP_RES, IFMT},  {OP_RES, IFMT},   {OP_RES, IFMT},  {OP_RES, IFMT},
  {OP_UNIMP, IFMT},{OP_SWC1, IFMT}, {OP_UNIMP, IFMT},{OP_UNIMP,IFMT},
  {OP_RES, IFMT},  {OP_RES, IFMT},   {OP_RES, IFMT},  {OP_RES, IFMT}
};

mips_opcode_t mips_special_table[] =
{
  OP_SLL,  OP_RES,   OP_SRL,  OP_SRA,  OP_SLLV,    OP_RES,   OP_SRLV, OP_SRAV,
  OP_JR,   OP_JALR,  OP_RES,  OP_RES,  OP_SYSCALL, OP_BREAK, OP_RES,  OP_RES,
  OP_MFHI, OP_MTHI,  OP_MFLO, OP_MTLO, OP_RES,     OP_RES,   OP_RES,  OP_RES,
  OP_MULT, OP_MULTU, OP_DIV,  OP_DIVU, OP_RES,     OP_RES,   OP_RES,  OP_RES,
  OP_ADD,  OP_ADDU,  OP_SUB,  OP_SUBU, OP_AND,     OP_OR,    OP_XOR,  OP_NOR,
  OP_RES,  OP_RES,   OP_SLT,  OP_SLTU, OP_RES,     OP_RES,   OP_RES,  OP_RES,
  OP_RES,  OP_RES,   OP_RES,  OP_RES,  OP_RES,     OP_RES,   OP_RES,  OP_RES,
  OP_RES,  OP_RES,   OP_RES,  OP_RES,  OP_RES,     OP_RES,   OP_RES,  OP_RES
};

mips_opcode_t mips_cop1_table[] =
{
  OP_FADD, OP_FSUB, OP_FMUL, OP_FDIV, OP_FSQRT, OP_FABS, OP_FMOV, OP_FNEG,
  OP_ROUND_L, OP_TRUNC_L, OP_CEIL_L, OP_FLOOR_L, OP_ROUND_W, OP_TRUNC_W, OP_CEIL_W,
  OP_FLOOR_W,
  /* 16-31, reserved */
  OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES,
  OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES,
  OP_CVT_S, OP_CVT_D, OP_RES, OP_RES, OP_CVT_W, OP_CVT_L,
  /* 38-47, reserved */
  OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES, OP_RES,
  /* 48-63, float compares */
  OP_C_F, OP_C_UN, OP_C_EQ, OP_C_UEQ, OP_C_OLT, OP_C_ULT, OP_C_OLE, OP_C_ULE,
  OP_C_SF, OP_C_NGLE, OP_C_SEQ, OP_C_NGL, OP_C_LT, OP_C_NGE, OP_C_LE, OP_C_NGT
};

const char *mips_op_strings[] =
{
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
  "??CP2", "??CP3", "break",
  /* FPU */
  "mfc1", "cfc1", "mtc1", "ctc1", "bc1f", "bc1t", "lwc1", "swc1",
  "fadd", "fsub", "fmul", "fdiv", "fsqrt", "fabs", "fmov", "fneg",
  "round.l", "trunc.l", "ceil.l", "floor.l", "ceil.w", "floor.w",
  "round.w", "trunc.w", "cvt.s", "cvt.d", "cvt.w", "cvt.l",
  "Reserved","Reserved","Reserved","Reserved","Reserved",
  "Reserved","Reserved","Reserved","Reserved",
  "c.f", "c.un", "c.eq", "c.ueq", "c.olt", "c.ult", "c.ole", "c.ule",
  "c.sf", "c.ngle", "c.seq", "c.ngl", "c.lt", "c.nge", "c.le", "c.ngt"
};

const char *mips_reg_strings[] =
{
  "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5",
  "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1",
  "gp", "sp", "s8", "ra", "hi", "lo",
  "RES","RES","RES","RES","RES","RES",
  "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13",
  "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24", "f25",
  "f26", "f27", "f28", "f29", "f30", "f31",
  "cpc", "cm0", "cm1", "cm2", "cm3", "cm4", "cm5", "cm6", "cm7", "madr", "ecb", "ear",
  "fna", "mem",
};

MIPS_register_t mips_caller_saved[] = { R_S0, R_S1, R_S2, R_S3, R_S4, R_S5, R_S6, R_S7, R_S8, R_ZERO };

static bool ifmt_zero_extend[ N_INSNS ];

static void __attribute__((constructor))init()
{
  ifmt_zero_extend[ OP_ANDI ] = true;
  ifmt_zero_extend[ OP_ORI ]  = true;
  ifmt_zero_extend[ OP_XORI ] = true;
}

bool mips_zero_extend_opcode(mips_opcode_t opcode)
{
  if (opcode > N_INSNS)
    return false;
  return ifmt_zero_extend[opcode];
}
