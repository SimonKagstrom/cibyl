/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      string-instruction.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Instruction encoding to string
 *
 * $Id:$
 *
 ********************************************************************/
#include <instruction.hh>
#include <mips.hh>

int instruction_to_string(Instruction *insn, char *buf, int buf_len)
{
  const char *insn_name = "unknown";
  uint32_t rs, rt, rd;
  uint32_t extra;
  int out = 0;

  if (insn->getOpcode() < SPECIAL)
	  insn_name = mips_op_strings[insn->getOpcode()];

  rs = insn->getRs();
  rt = insn->getRt();
  rd = insn->getRd();
  extra = insn->getExtra();

  switch(insn->getOpcode())
    {
      /* rfmt */
    case OP_ADDU:
    case OP_ADD:
    case OP_SUB:
    case OP_SUBU:
    case OP_XOR:
    case OP_AND:
    case OP_OR:
    case OP_NOR:
    case OP_SRAV:
    case OP_SLLV:
    case OP_SRLV:
    case OP_SLTU:
    case OP_SLT:
      out = snprintf(buf, buf_len, "%s %s, %s, %s", insn_name, mips_reg_strings[rd],
	       mips_reg_strings[rs], mips_reg_strings[rt]);
      break;
    case OP_SLL:
    case OP_SRA:
    case OP_SRL:
      out = snprintf(buf, buf_len, "%s %s, %s, %d", insn_name, mips_reg_strings[rd],
	       mips_reg_strings[rt], (int32_t)extra);
      break;
      /* Ifmt */
    case OP_SLTIU:
    case OP_SLTI:
    case OP_ADDI:
    case OP_ADDIU:
    case OP_XORI:
    case OP_ANDI:
    case OP_ORI:
      out = snprintf(buf, buf_len, "%s %s, %s, %d", insn_name, mips_reg_strings[rt],
	       mips_reg_strings[rs], (int32_t)extra);
      break;
      /* Jfmt */
    case OP_BGEZAL:
    case OP_JAL:
      out = snprintf(buf, buf_len, "%s 0x%x", insn_name, (extra<<2));
      break;
    case OP_JR:
    case OP_JALR:
      out = snprintf(buf, buf_len, "%s %s", insn_name, mips_reg_strings[rs]);
      break;
    case OP_J:
      out = snprintf(buf, buf_len, "%s 0x%x", insn_name, (extra<<2));
      break;

      /* Memory handling */
    case OP_LW:
    case OP_SW:
    case OP_LB:
    case OP_LBU:
    case OP_LH:
    case OP_LHU:
    case OP_LWL:
    case OP_LWR:
    case OP_SB:
    case OP_SH:
    case OP_SWL:
    case OP_SWR:
      out = snprintf(buf, buf_len, "%s %s, %d(%s)", insn_name, mips_reg_strings[rt],
               extra, mips_reg_strings[rs]);
      break;

      /* Misc other instructions */
    case OP_BREAK:
      out = snprintf(buf, buf_len, "break");
      break;
    case OP_MULT:
    case OP_MULTU:
    case OP_DIV:
    case OP_DIVU:
      out = snprintf(buf, buf_len, "%s %s, %s", insn_name, mips_reg_strings[rs],
               mips_reg_strings[rt]);
      break;
    case OP_MFLO:
    case OP_MFHI:
    case OP_MTLO:
    case OP_MTHI:
      out = snprintf(buf, buf_len, "%s %s", insn_name, mips_reg_strings[rs]);
      break;
    case OP_LUI:
      out = snprintf(buf, buf_len, "%s %s, 0x%x", insn_name, mips_reg_strings[rt], extra << 16);
      break;

      /* Conditional jumps, shifts */
    case OP_BEQ:
    case OP_BNE:
      out = snprintf(buf, buf_len, "%s %s, %s, 0x%x", insn_name, mips_reg_strings[rs], mips_reg_strings[rt],
               (insn->getAddress() + 4) + (extra << 2));
      break;
    case OP_BGEZ:
    case OP_BGTZ:
    case OP_BLEZ:
    case OP_BLTZ:
      out = snprintf(buf, buf_len, "%s %s, 0x%x", insn_name, mips_reg_strings[rs],
               (insn->getAddress() + 4) + (extra << 2));
      break;
    case CIBYL_SYSCALL:
        out = snprintf(buf, buf_len, "cibyl_sysc 0x%x", extra);
        break;
    case CIBYL_REGISTER_ARGUMENT:
        out = snprintf(buf, buf_len, "cibyl_sysc_arg 0x%x", extra);
        break;
    default:
      out = snprintf(buf, buf_len, "Unknown instruction 0x%x", insn->getOpcode());
    }

  panic_if(out < 0 || out >= buf_len,
           "buffer to small to write instruction string\n");

  return out;
}
