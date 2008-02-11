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
  {OP_UNIMP, IFMT},{OP_UNIMP, IFMT}, {OP_UNIMP, IFMT},{OP_UNIMP,IFMT},
  {OP_RES, IFMT},  {OP_RES, IFMT},   {OP_RES, IFMT},  {OP_RES, IFMT},
  {OP_UNIMP, IFMT},{OP_UNIMP, IFMT}, {OP_UNIMP, IFMT},{OP_UNIMP,IFMT},
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
  "??CP2", "??CP3", "break"
};

const char *mips_reg_strings[] =
{
  "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5",
  "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1",
  "gp", "sp", "s8", "ra", "hi", "lo", "cpc", "cm0", "cm1", "cm2", "cm3", "cm4", "cm5",
  "cm6", "cm7", "cm8", "ecb", "ear", "fna", "mem",
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
