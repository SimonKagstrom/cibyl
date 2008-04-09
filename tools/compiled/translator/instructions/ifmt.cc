/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      ifmt.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Ifmt instructions
 *
 * $Id:$
 *
 ********************************************************************/

class Ifmt : public Instruction
{
public:
  Ifmt(const char *what, uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd, int32_t extra) : Instruction(address, opcode, rs, rt, rd, extra)
  {
    this->bc = what;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs);
    emit->bc_pushconst( this->extra );
    emit->bc_generic_insn( this->bc );
    emit->bc_popregister( this->rt);

    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rt, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p);
  };


protected:
  const char *bc;
};

class Addi : public Ifmt
{
public:
  Addi(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd, int32_t extra) : Ifmt("iadd", address, opcode, rs, rt, rd, extra)
  {
  }

  bool pass2()
  {
    int extra = signext_16(this->extra);

    /* Partially from NestedVM */
    if (this->rs == R_ZERO)
      {
	emit->bc_pushconst(this->extra);
	emit->bc_popregister(this->rt);
      }
    else if (this->rt == this->rs && (extra >= -32768 && extra <= 32767) &&
	     !regalloc->regIsStatic(this->rt))
      emit->bc_iinc(this->rt, extra);
    else
      return Ifmt::pass2();

    return true;
  }
};

class Lui : public Instruction
{
public:
  Lui(uint32_t address, int opcode, MIPS_register_t rt, int32_t extra) :
    Instruction(address, opcode, R_ZERO, rt, R_ZERO, extra)
  {
    this->extra = this->extra & 0xffff;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    uint32_t v = (uint32_t)this->extra;

    emit->bc_pushconst_u( v << 16 );
    emit->bc_popregister( this->rt );

    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rt, p);
  }
};
