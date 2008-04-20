/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      fpu.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:
 *
 * $Id:$
 *
 ********************************************************************/
class CompFmtFloat : public Instruction
{
public:
  CompFmtFloat(int fmt, uint32_t address, int opcode,
               MIPS_register_t fs, MIPS_register_t ft, MIPS_register_t fd) : Instruction(address, opcode, fs, ft, fd, 0)
  {
    const char *f_bc = "err";

    switch (opcode)
      {
      case OP_FADD:
        f_bc = "fadd";
        break;
      case OP_FSUB:
        f_bc = "fsub";
        break;
      case OP_FMUL:
        f_bc = "fmul";
        break;
      case OP_FDIV:
        f_bc = "fdiv";
        break;
      default:
        /* Should never happen, this is an error */
        emit->error("Unknown FPU arithmetic opcode %x\n",
                    opcode);
        exit(1);
        break;
      }

    /* rs,rt,rd are reused */
    this->bc = f_bc;
    this->fmt = fmt;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->warning("Instructions CompFmt for %s not yet implemented\n", this->bc);
    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rd, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(this->rt, p) ;
  };

protected:
  int fmt;
  const char *bc;
};

class CompFmtDouble : public Instruction
{
public:
  CompFmtDouble(int fmt, uint32_t address, int opcode,
                MIPS_register_t fs, MIPS_register_t ft, MIPS_register_t fd) : Instruction(address, opcode, fs, ft, fd, 0)
  {
    const char *bc = "err";

    switch (opcode)
      {
      case OP_FADD:
        bc = "dadd";
        break;
      case OP_FSUB:
        bc = "dsub";
        break;
      case OP_FMUL:
        bc = "dmul";
        break;
      case OP_FDIV:
        bc = "ddiv";
        break;
      default:
        /* Should never happen, this is an error */
        emit->error("Unknown FPU arithmetic opcode %x\n",
                    opcode);
        exit(1);
        break;
      }

    /* rs,rt,rd are reused */
    this->bc = bc;
    this->fmt = fmt;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->warning("Instructions CompFmt for %s not yet implemented\n", this->bc);
    return true;
  }

  int fillDestinations(int *p)
  {
    /* FIXME! How to handle this??? */
    return this->addToRegisterUsage(this->rd, p) + this->addToRegisterUsage( (MIPS_register_t)((int)this->rd + 1), p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage( (MIPS_register_t)((int)this->rs + 1), p) +
      this->addToRegisterUsage(this->rt, p) + this->addToRegisterUsage( (MIPS_register_t)((int)this->rt + 1), p) ;
  };

protected:
  int fmt;
  const char *bc;
};

class Cvt_w : public Instruction
{
public:
  Cvt_w(uint32_t address, int opcode, int fmt,
        MIPS_register_t fs, MIPS_register_t fd) : Instruction(address, opcode, fs, R_ZERO, fd, 0)
  {
    this->fmt = fmt;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->warning("Instructions Cvt_w not yet implemented\n");
    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rd, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p);
  };

protected:
  int fmt;
};

class CmpFmtFloat : public Instruction
{
public:
  CmpFmtFloat(uint32_t address, int opcode,
               MIPS_register_t fs, MIPS_register_t ft) : Instruction(address, opcode, fs, ft, R_ZERO, 0)
  {
    switch (opcode)
      {
      case OP_C_ULE:
        this->bc = "maboo"; /* FIXME! Actually implement :-)*/
        break;
      default:
        /* Should never happen, this is an error */
        emit->error("Unknown FPU compare opcode %x\n",
                    opcode);
        exit(1);
        break;
      }

    this->fmt = fmt;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->warning("Instructions CmpFmtFloat for %s not yet implemented\n", this->bc);
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(this->rt, p) ;
  };

protected:
  int fmt;
  const char *bc;
};

class CmpFmtDouble : public Instruction
{
public:
  CmpFmtDouble(uint32_t address, int opcode,
               MIPS_register_t fs, MIPS_register_t ft) : Instruction(address, opcode, fs, ft, R_ZERO, 0)
  {
    switch (opcode)
      {
      case OP_C_ULE:
        this->bc = "maboo"; /* FIXME! Actually implement :-)*/
        break;
      default:
        /* Should never happen, this is an error */
        emit->error("Unknown FPU compare opcode %x\n",
                    opcode);
        exit(1);
        break;
      }

    this->fmt = fmt;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->warning("Instructions CompFmt for %s not yet implemented\n", this->bc);
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage( (MIPS_register_t)((int)this->rs + 1), p) +
      this->addToRegisterUsage(this->rt, p) + this->addToRegisterUsage( (MIPS_register_t)((int)this->rt + 1), p) ;
  };

protected:
  int fmt;
  const char *bc;
};
