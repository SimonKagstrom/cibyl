/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      mips.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   MIPS stuff
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __MIPS_HH__
#define __MIPS_HH__

#include <stdint.h>

typedef enum
{
  R_ZERO = 0,
  R_AT = 1,
  R_V0 = 2,
  R_V1 = 3,
  R_A0 = 4,
  R_A1 = 5,
  R_A2 = 6,
  R_A3 = 7,
  R_T0 = 8,
  R_T1 = 9,
  R_T2 = 10,
  R_T3 = 11,
  R_T4 = 12,
  R_T5 = 13,
  R_T6 = 14,
  R_T7 = 15,
  R_S0 = 16,
  R_S1 = 17,
  R_S2 = 18,
  R_S3 = 19,
  R_S4 = 20,
  R_S5 = 21,
  R_S6 = 22,
  R_S7 = 23,
  R_T8 = 24,
  R_T9 = 25,
  R_K0 = 26,
  R_K1 = 27,
  R_GP = 28,
  R_SP = 29,
  R_S8 = 30,
  R_RA = 31,
  R_HI = 32,
  R_LO = 33,

  /* Special registers */
  R_CPC = 34,
  R_CM0 = 35,
  R_CM1 = 36,
  R_CM2 = 37,
  R_CM3 = 38,
  R_CM4 = 39,
  R_CM5 = 40,
  R_CM6 = 41,
  R_CM7 = 42,
  R_CM8 = 43,
  R_ECB = 44,
  R_EAR = 45,
  R_FNA = 46,
  R_MEM = 47
} MIPS_register_t;

#define N_REGS 48

typedef enum
{
  OP_ADD    = 1,
  OP_ADDI   = 2,
  OP_ADDIU  = 3,
  OP_ADDU   = 4,
  OP_AND    = 5,
  OP_ANDI   = 6,
  OP_BEQ    = 7,
  OP_BGEZ   = 8,
  OP_BGEZAL = 9,
  OP_BGTZ   = 10,
  OP_BLEZ   = 11,
  OP_BLTZ   = 12,
  OP_BLTZAL = 13,
  OP_BNE    = 14,
  OP_DIV    = 16,
  OP_DIVU   = 17,
  OP_J	    = 18,
  OP_JAL    = 19,
  OP_JALR   = 20,
  OP_JR     = 21,
  OP_LB     = 22,
  OP_LBU    = 23,
  OP_LH     = 24,
  OP_LHU    = 25,
  OP_LUI    = 26,
  OP_LW     = 27,
  OP_LWL    = 28,
  OP_LWR    = 29,
  OP_MFHI   = 31,
  OP_MFLO   = 32,
  OP_MTHI   = 34,
  OP_MTLO   = 35,
  OP_MULT   = 36,
  OP_MULTU  = 37,
  OP_NOR    = 38,
  OP_OR	    = 39,
  OP_ORI    = 40,
  OP_RFE    = 41,
  OP_SB     = 42,
  OP_SH     = 43,
  OP_SLL    = 44,
  OP_SLLV   = 45,
  OP_SLT    = 46,
  OP_SLTI   = 47,
  OP_SLTIU  = 48,
  OP_SLTU   = 49,
  OP_SRA    = 50,
  OP_SRAV   = 51,
  OP_SRL    = 52,
  OP_SRLV   = 53,
  OP_SUB    = 54,
  OP_SUBU   = 55,
  OP_SW     = 56,
  OP_SWL    = 57,
  OP_SWR    = 58,
  OP_XOR    = 59,
  OP_XORI   = 60,
  OP_SYSCALL= 61,
  OP_UNIMP  = 62,
  OP_RES    = 63,
  OP_MFCP0  = 65,
  OP_MTCP0  = 66,
  OP_CFCP0  = 67,
  OP_CTCP0  = 68,
  OP_BCCP0  = 69,
  OP_TLBR   = 70,
  OP_TLBWI  = 71,
  OP_TLBWR  = 72,
  OP_TLBP   = 73,
  OP_COP1   = 75,
  OP_COP2   = 76,
  OP_COP3   = 77,
  OP_BREAK  = 78,

  SPECIAL = 100,
  BCOND = 101,
  COP0 = 102,
  COP1 = 103,
  COP2 = 104,
  COP3 = 105,
  CIBYL_SYSCALL = 999, /* Needed? */
  CIBYL_REGISTER_ARGUMENT = 1000,
  CIBYL_ASSIGN_MEMREG = 1001,
} mips_opcode_t;

#define N_INSNS 79

typedef enum
{
  IFMT = 1,
  JFMT = 2,
  RFMT = 3
} mips_fmt_t;

typedef struct
{
  mips_opcode_t type;
  mips_fmt_t fmt;
} mips_op_entry_t;

extern mips_op_entry_t mips_op_entries[];

extern mips_opcode_t mips_special_table[];

extern const char *mips_op_strings[];

extern const char *mips_reg_strings[];

/**
 * Zero-terminated list of caller-saved MIPS registers
 */
extern MIPS_register_t mips_caller_saved[];

bool mips_zero_extend_opcode(mips_opcode_t opcode);

static inline MIPS_register_t mips_encoding_get_rs(uint32_t word)
{
  return (MIPS_register_t)((word >> 21) & 0x1f);
}

static inline MIPS_register_t mips_encoding_get_rt(uint32_t word)
{
  return (MIPS_register_t)((word >> 16) & 0x1f);
}

static inline MIPS_register_t mips_encoding_get_rd(uint32_t word)
{
  return (MIPS_register_t)((word >> 11) & 0x1f);
}

#endif /* !__MIPS_HH__ */
