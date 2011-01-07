/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      instruction.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Insn implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <assert.h>
#include <stdio.h>

#include <instruction.hh>
#include <controller.hh>
#include <javaclass.hh>
#include <emit.hh>
#include <config.hh>
#include <utils.h>

/* The base class for all instructions */
Instruction::Instruction(uint32_t address, int opcode,
			 MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd,
			 int32_t extra)
{
  this->address = address;
  this->size = 4;
  this->opcode = opcode;
  this->rs = rs;
  this->rt = rt;
  this->rd = rd;
  this->extra = extra;
  this->branchTarget = false;
  this->prefix = NULL;
  this->delayed = NULL;

  memset(this->prev_register_reads, 0, sizeof(this->prev_register_reads));
  memset(this->prev_register_writes, 0, sizeof(this->prev_register_writes));
  memset(this->next_register_reads, 0, sizeof(this->prev_register_reads));
  memset(this->next_register_writes, 0, sizeof(this->prev_register_writes));
};

Instruction::~Instruction()
{
}

void Instruction::setBranchTarget()
{
  this->branchTarget = true;
}

/* Implementation of all instructions (OK, slightly ugly to just
 *  include but it's simple!)
 */
#include "instructions/ifmt.cc"
#include "instructions/rfmt.cc"
#include "instructions/muldiv.cc"
#include "instructions/set.cc"
#include "instructions/jfmt.cc"
#include "instructions/nop.cc"
#include "instructions/syscall.cc"
#include "instructions/memory.cc"
#include "instructions/fpu.cc"
#include "instructions/catch.cc"

/* --- The factory --- */
Instruction *InstructionFactory::create(uint32_t address, uint32_t word)
{
  MIPS_register_t rs, rt, rd;
  int32_t extra;
  mips_op_entry_t opentry;
  mips_opcode_t opcode;
  int cp1_fmt, cp1_func;
  MIPS_register_t cp1_fs, cp1_ft, cp1_fd;

  word = be32_to_host(word);

  /* Split the instruction (not all these will be used) */
  rs = mips_encoding_get_rs(word);
  rt = mips_encoding_get_rt(word);
  rd = mips_encoding_get_rd(word);
  cp1_fmt = mips_encoding_get_cp1_fmt(word);
  cp1_func = mips_encoding_get_cp1_func(word);
  cp1_fs = mips_encoding_get_fs(word);
  cp1_ft = mips_encoding_get_ft(word);
  cp1_fd = mips_encoding_get_fd(word);

  opentry = mips_op_entries[(word >> 26) & 0x3f];
  opcode = opentry.type; /* Assume type is the opcode */

  extra = 0;

  /* Syscall */
  if ( (word >> 24) == 0xff )
    return new SyscallInsn(address, word & 0x00ffffff);

  /* Syscall argument - always registers */
  if ( (word >> 16) == 0xfefe )
    return new SyscallRegisterArgument(address, word & 0x0000001f);

  /* Catch statements */
  if ( (word >> 24) == 0xfd )
    return new CatchInsn(address, word & 0x00ffffff);

  if ( opentry.fmt == IFMT )
    {
      extra = word & 0xffff;
      if ( !mips_zero_extend_opcode(opcode) && (extra & 0x8000) == 0x8000 )
	extra = ( -1 & ~0xffff ) | extra;
    }
  else if ( opentry.fmt == RFMT )
    extra = (word >> 6) & 0x1f;
  else /* JFMT */
    extra = (word & 0x03ffffff);

  if ( opentry.type == SPECIAL )
    opcode = mips_special_table[word & 0x3f];
  else if ( opentry.type == BCOND )
    {
      switch(word & 0x1f0000)
	{
	case 0:
	  opcode = OP_BLTZ; break;
	case 0x10000:
	  opcode = OP_BGEZ; break;
	case 0x100000:
	  opcode = OP_BLTZAL; break;
	case 0x110000:
	  opcode = OP_BGEZAL; break;
	default:
	  opcode = OP_UNIMP; break;
	}
    }
  else if ( opentry.type == COP1 ) /* FPU instructions */
    {
      opcode = mips_cop1_table[cp1_func];
    }

  /* Now we have extra, rs, rt, rd and opcode correctly setup! */
  switch(opcode)
    {
    case OP_ADDU: return new Addu(address, opcode,
				  rs, rt, rd);
    case OP_ADD: return new Rfmt("iadd", address, opcode,
				 rs, rt, rd);
    case OP_SUB: return new Rfmt("isub", address, opcode,
				 rs, rt, rd);
    case OP_SUBU: return new Subu(address, opcode,
				  rs, rt, rd);
    case OP_XOR: return new Rfmt("ixor", address, opcode,
				 rs, rt, rd);
    case OP_AND: return new Rfmt("iand", address, opcode,
				 rs, rt, rd);
    case OP_OR: return new Rfmt("ior", address, opcode,
				rs, rt, rd);
    case OP_NOR: return new Nor(address, opcode,
				rs, rt, rd);
    case OP_SRAV: return new ShiftInstructionV("ishr", address, opcode,
					       rs, rt, rd);
    case OP_SLLV: return new ShiftInstructionV("ishl", address, opcode,
					       rs, rt, rd);
    case OP_SRLV: return new ShiftInstructionV("iushr", address, opcode,
					       rs, rt, rd);
    case OP_SLL: return new ShiftInstruction("ishl", address, opcode,
					     rs, rt, rd, extra);
    case OP_SRA: return new ShiftInstruction("ishr", address, opcode,
					     rs, rt, rd, extra);
    case OP_SRL: return new ShiftInstruction("iushr", address, opcode,
					     rs, rt, rd, extra);
      /* Ifmt */
    case OP_ADDI: return new Addi(address, opcode,
				  rs, rt, rd, extra);
    case OP_ADDIU: return new Addi(address, opcode,
				   rs, rt, rd, extra);
    case OP_XORI: return new Ifmt("ixor", address, opcode,
				  rs, rt, rd, extra);
    case OP_ANDI: return new Ifmt("iand", address, opcode,
				  rs, rt, rd, extra);
    case OP_ORI: return new Ifmt("ior", address, opcode,
				 rs, rt, rd, extra);

      /* Jfmt */
    case OP_JAL: return new Jal(address, opcode, extra);
    case OP_JALR: return new Jalr(address, opcode, rs);
    case OP_J: return new Jump(address, opcode, extra);
    case OP_BGEZAL: return new Jal(address, opcode, extra); /* BAL instructions */

      /* Memory handling */
    case OP_LW: return new Lw(address, opcode, rs, rt, extra);
    case OP_SW: return new Sw(address, opcode, rs, rt, extra);
    case OP_LB: return new Lb(address, opcode, rs, rt, extra);
    case OP_LBU: return new Lbu(address, opcode, rs, rt, extra);
    case OP_LH:  return new Lh(address, opcode, rs, rt, extra);
    case OP_LHU: return new Lhu(address, opcode, rs, rt, extra);
    case OP_LWL: return new LoadXX("WordLeft", address, opcode,
				   rs, rt, extra);
    case OP_LWR: return new Nop(address);
    case OP_SB: return new Sb(address, opcode, rs, rt, extra);
    case OP_SH: return new Sh(address, opcode, rs, rt, extra);
    case OP_SWL: return new StoreXX("WordLeft", address, opcode,
				    rs, rt, extra);
    case OP_SWR: return new Nop(address);

      /* Misc other instructions */
    case OP_BREAK: return new Nop(address);
    case OP_MULT: return new Mult(address, opcode, rs, rt);
    case OP_MULTU: return new MulDiv("multu", address, opcode, rs, rt, rd, extra);
    case OP_DIV: return new Div(address, opcode, rs, rt);
    case OP_DIVU: return new MulDiv("divu", address, opcode, rs, rt, rd, extra);
    case OP_MFLO: return new Mfxx(address, opcode, rd, R_LO);
    case OP_MFHI: return new Mfxx(address, opcode, rd, R_HI);
    case OP_MTLO: return new Mtxx(address, opcode, rd, R_LO);
    case OP_MTHI: return new Mtxx(address, opcode, rd, R_HI);
    case OP_JR: return new Jr(address, opcode, rs); /* R-type, special */
    case OP_LUI: return new Lui(address, opcode, rt, extra);

      /* Conditional jumps, shifts */
    case OP_SLTU: return new Sltu(address, opcode, rs, rt, rd);
    case OP_SLT: return new Slt(address, opcode, rs, rt, rd);
    case OP_SLTIU: return new Sltiu(address, opcode, rs, rt, extra);
    case OP_SLTI: return new Slti(address, opcode, rs, rt, extra);

    case OP_BEQ: if(rt==R_ZERO)
		   return new OneRegisterConditionalJump("ifeq", address, opcode, rs, extra);
		 else
	           return new TwoRegisterConditionalJump("if_icmpeq", address, opcode, rs, rt, extra);
    case OP_BNE: if(rt==R_ZERO)
		   return new OneRegisterConditionalJump("ifne", address, opcode, rs, extra);
		 else
                   return new TwoRegisterConditionalJump("if_icmpne", address, opcode, rs, rt, extra);

    case OP_BGEZ: return new OneRegisterConditionalJump("ifge", address, opcode, rs, extra);
    case OP_BGTZ: return new OneRegisterConditionalJump("ifgt", address, opcode, rs, extra);
    case OP_BLEZ: return new OneRegisterConditionalJump("ifle", address, opcode, rs, extra);
    case OP_BLTZ: return new OneRegisterConditionalJump("iflt", address, opcode, rs, extra);

      /* FPU insns */
    case OP_LWC1: return new LWc1(address, opcode, rs,
				  mips_int_to_fpu_reg(rt), extra);
    case OP_SWC1: return new SWc1(address, opcode, rs,
				  mips_int_to_fpu_reg(rt), extra);
    case OP_CVT_W: return new Cvt_w(address, opcode, cp1_fmt,
                                    cp1_fs, cp1_fd);
    case OP_FADD:
    case OP_FSUB:
    case OP_FMUL:
    case OP_FDIV:
      if ( mips_cp1_fmt_is_double(cp1_fmt) )
        return new CompFmtDouble(cp1_fmt, address, opcode,
                                 cp1_fs, cp1_ft, cp1_fd);
      else
        return new CompFmtFloat(cp1_fmt, address, opcode,
                                cp1_fs, cp1_ft, cp1_fd);
    case OP_C_F:
    case OP_C_UN:
    case OP_C_EQ:
    case OP_C_UEQ:
    case OP_C_OLT:
    case OP_C_ULT:
    case OP_C_OLE:
    case OP_C_ULE:
    case OP_C_SF:
    case OP_C_NGLE:
    case OP_C_SEQ:
    case OP_C_NGL:
    case OP_C_LT:
    case OP_C_NGE:
    case OP_C_LE:
    case OP_C_NGT:
      if ( mips_cp1_fmt_is_double(cp1_fmt) )
        return new CmpFmtDouble(address, opcode, cp1_fs, cp1_ft);
      else
        return new CmpFmtFloat(address, opcode, cp1_fs, cp1_ft);
    case OP_MFC_1:
    case OP_CFC_1:
    case OP_MTC_1:
    case OP_CTC_1:
    case OP_BC1F:
    case OP_BC1T:
    case OP_FSQRT:
    case OP_FABS:
    case OP_FMOV:
    case OP_FNEG:
    case OP_CVT_S:
    case OP_CVT_D:
    case OP_CVT_L:
    case OP_ROUND_L: /* I think these are from MIPS II and up */
    case OP_TRUNC_L:
    case OP_CEIL_L:
    case OP_FLOOR_L:
    case OP_CEIL_W:
    case OP_FLOOR_W:
    case OP_ROUND_W:
    case OP_TRUNC_W:
      panic("Instruction %s on address 0x%08x not yet implemented\n:",
            mips_op_strings[opcode], address);
    default:
      break;
    }

  panic("Pattern 0x%x on address 0x%x is not an instruction\n",
        word, address);

  return NULL;
}

InstructionFactory *InstructionFactory::instance;

InstructionFactory *InstructionFactory::getInstance()
{
  if (!InstructionFactory::instance)
    InstructionFactory::instance = new InstructionFactory();

  return InstructionFactory::instance;
}

Instruction *InstructionFactory::createNop(uint32_t address)
{
  return new Nop(address);
}

Instruction *InstructionFactory::createDelaySlotNop(uint32_t address)
{
  return new DelaySlotNop(address);
}

Instruction *InstructionFactory::createTailCallJump(uint32_t address, uint32_t extra)
{
  return new JalReturn(address, OP_JAL, extra);
}
