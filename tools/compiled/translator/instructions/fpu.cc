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
class CompFmt : public Instruction
{
public:
  CompFmt(const char *f_bc, const char *d_bc, int fmt, uint32_t address, int opcode,
          MIPS_register_t fs, MIPS_register_t ft, MIPS_register_t fd) : Instruction(address, opcode, fs, ft, fd, 0)
  {
    /* rs,rt,rd are reused */
    this->float_bc = f_bc;
    this->double_bc = d_bc;
    this->fmt = fmt;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->warning("Instructions CompFmt for %s not yet implemented\n", this->float_bc);
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
  const char *float_bc;
  const char *double_bc;
};

class Cvt_w : public Instruction
{
public:
  Cvt_w(int fmt, uint32_t address, int opcode,
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
  const char *float_bc;
  const char *double_bc;
};
