/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      muldiv.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Multiplications and divisions
 *
 * $Id:$
 *
 ********************************************************************/

class MulDiv : public Instruction
{
public:
  MulDiv(const char *what, uint32_t address, int opcode,
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
    JavaMethod *my = controller->getMethodByAddress(this->address);

    if (!my)
      {
	emit->error("The instruction at 0x%x has no method\n",
		    this->getAddress());
	return false;
      }
    /* FIXME! Check if this instruction -only- uses HI/LO and only
     * generate 32 bits if so
     */

    emit->bc_pushregister( this->rs );
    emit->bc_pushregister( this->rt );
    emit->bc_invokestatic( "CRunTime/%s(II)J", this->bc);
    emit->bc_dup2();
    emit->bc_pushconst(32);
    emit->bc_lushr();
    emit->bc_l2i();
    emit->bc_popregister( R_HI );
    emit->bc_l2i();
    emit->bc_popregister( R_LO );
    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_LO, p) + this->addToRegisterUsage(R_HI, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(this->rt, p);
  };

  int getMaxStackHeight()
  {
    return 6;
  }
private:
  const char *bc;
};

class Mult : public MulDiv
{
public:
  Mult(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt) : MulDiv("", address, opcode, rs, rt, R_ZERO, 0)
  {
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs );
    emit->bc_i2l( );
    emit->bc_pushregister( this->rt );
    emit->bc_i2l( );
    emit->bc_lmul( );
    emit->bc_dup2( );
    emit->bc_pushconst( 32 );
    emit->bc_lushr( );
    emit->bc_l2i( );
    emit->bc_popregister( R_HI );
    emit->bc_l2i( );
    emit->bc_popregister( R_LO );
    return true;
  }
};


class Div : public MulDiv
{
public:
  Div(uint32_t address, int opcode,
      MIPS_register_t rs, MIPS_register_t rt) : MulDiv("", address, opcode, rs, rt, R_ZERO, 0)
  {
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs );
    emit->bc_pushregister( this->rt );
    emit->bc_dup2( );
    emit->bc_idiv( );
    emit->bc_popregister( R_LO );
    emit->bc_irem( );
    emit->bc_popregister( R_HI );
    return true;
  }
};



class Mfxx : public Instruction
{
public:
  Mfxx(uint32_t address, int opcode,
       MIPS_register_t rd, MIPS_register_t src) : Instruction(address, opcode, src, R_ZERO, rd, 0)
  {
    this->src = src;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister( this->src );
    emit->bc_popregister( this->rd );
    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rd, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->src, p);
  };
private:
  MIPS_register_t src;
};


class Mtxx : public Instruction
{
public:
  Mtxx(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t dst) : Instruction(address, opcode, rs, R_ZERO, R_ZERO, 0)
  {
    this->dst = dst;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs );
    emit->bc_popregister( this->dst );
    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rd, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->dst, p);
  };
private:
  MIPS_register_t dst;
};


class OneRegisterSetInstruction : public Instruction
{
public:
  OneRegisterSetInstruction(const char *what, uint32_t address, int opcode,
			    MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : Instruction(address, opcode, rs, rt, R_ZERO, extra)
  {
    this->bc = what;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs );
    emit->bc_pushconst( this->extra );
    emit->bc_invokestatic("CRunTime/%s(II)I", this->bc);
    emit->bc_popregister( this->rt );
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
private:
  const char *bc;
};
