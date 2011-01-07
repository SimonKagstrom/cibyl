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

  R_F0  = 40,
  R_F1  = 41,
  R_F2  = 42,
  R_F3  = 43,
  R_F4  = 44,
  R_F5  = 45,
  R_F6  = 46,
  R_F7  = 47,
  R_F8  = 48,
  R_F9  = 49,
  R_F10 = 50,
  R_F11 = 51,
  R_F12 = 52,
  R_F13 = 53,
  R_F14 = 54,
  R_F15 = 55,
  R_F16 = 56,
  R_F17 = 57,
  R_F18 = 58,
  R_F19 = 59,
  R_F20 = 60,
  R_F21 = 61,
  R_F22 = 62,
  R_F23 = 63,
  R_F24 = 64,
  R_F25 = 65,
  R_F26 = 66,
  R_F27 = 67,
  R_F28 = 68,
  R_F29 = 69,
  R_F30 = 70,
  R_F31 = 71,

  /* Special registers */
  R_CPC = 72,
  R_CM0 = 73,
  R_CM1 = 74,
  R_CM2 = 75,
  R_CM3 = 76,
  R_CM4 = 77,
  R_CM5 = 78,
  R_CM6 = 79,
  R_CM7 = 80,
  R_MADR= 81, /* Memory address temp for jsr lb/lh/sb/sh */
  R_ECB = 82, /* Exception call back address */
  R_EAR = 83, /* Exception argument (to function) */
  R_FNA = 84, /* Function index (for multi-function methods) */
  R_MEM = 85  /* Virtual memory "register" */
} MIPS_register_t;

#define N_REGS 86

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

  /* FPU ops */
  OP_MFC_1  = 79,
  OP_CFC_1  = 80,
  OP_MTC_1  = 81,
  OP_CTC_1  = 82,
  OP_BC1F   = 83,
  OP_BC1T   = 84,
  OP_LWC1   = 85,
  OP_SWC1   = 86,
  OP_FADD   = 87,
  OP_FSUB   = 88,
  OP_FMUL   = 89,
  OP_FDIV   = 90,
  OP_FSQRT  = 91,
  OP_FABS   = 92,
  OP_FMOV   = 93,
  OP_FNEG   = 94,
  OP_ROUND_L= 95,
  OP_TRUNC_L= 96,
  OP_CEIL_L = 98,
  OP_FLOOR_L= 99,
  OP_CEIL_W = 100,
  OP_FLOOR_W= 101,
  OP_ROUND_W= 102,
  OP_TRUNC_W= 103,
  OP_CVT_S  = 104,
  OP_CVT_D  = 105,
  OP_CVT_W  = 106,
  OP_CVT_L  = 107,
  OP_C_F    = 108,
  OP_C_UN   = 109,
  OP_C_EQ   = 110,
  OP_C_UEQ  = 111,
  OP_C_OLT  = 112,
  OP_C_ULT  = 113,
  OP_C_OLE  = 114,
  OP_C_ULE  = 115,
  OP_C_SF   = 116,
  OP_C_NGLE = 117,
  OP_C_SEQ  = 118,
  OP_C_NGL  = 119,
  OP_C_LT   = 120,
  OP_C_NGE  = 121,
  OP_C_LE   = 122,
  OP_C_NGT  = 123,

  SPECIAL   = 124,
  BCOND     = 125,
  COP0      = 126,
  COP1      = 127,
  COP2      = 128,
  COP3      = 129,
  CIBYL_SYSCALL = 130, /* Needed? */
  CIBYL_REGISTER_ARGUMENT = 131,
  CIBYL_ASSIGN_MEMREG = 132,
} mips_opcode_t;

#define N_INSNS 133

/* Indices */
typedef enum
{
  I_RS = 0,
  I_RT = 1,
  I_RD = 2,
} mips_register_type_t;

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

extern mips_opcode_t mips_cop1_table[];

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

static inline MIPS_register_t mips_encoding_get_fs(uint32_t word)
{
  return (MIPS_register_t)( ((int)mips_encoding_get_rs(word)) + R_F0);
}

static inline MIPS_register_t mips_encoding_get_ft(uint32_t word)
{
  return (MIPS_register_t)( ((int)mips_encoding_get_rt(word)) + R_F0);
}

static inline MIPS_register_t mips_encoding_get_fd(uint32_t word)
{
  return (MIPS_register_t)( ((word >> 6) & 0x1f) + R_F0);
}

static inline int mips_encoding_get_cp1_fmt(uint32_t word)
{
  return (word >> 21) & 0x1f;
}

static inline int mips_encoding_get_cp1_func(uint32_t word)
{
  return word & 0x3f;
}

static inline MIPS_register_t mips_int_to_fpu_reg(int v)
{
  return (MIPS_register_t)(v + R_F0);
}

static inline bool mips_cp1_fmt_is_double(int fmt)
{
  return (fmt & 1) == 1;
}

#endif /* !__MIPS_HH__ */
