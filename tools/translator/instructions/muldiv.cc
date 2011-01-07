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
    this->using_hi = false;
    this->using_lo = 0x123;
  }

  bool pass1()
  {
    JavaMethod *my = controller->getMethodByAddress(this->address);

    panic_if(!my,
             "The mul/div instruction at 0x%x has no method\n",
             this->getAddress());

    if (my->opcodeIsUsed(OP_MFHI) || my->opcodeIsUsed(OP_MTHI))
      this->using_hi = true;

    if (my->opcodeIsUsed(OP_MFLO) || my->opcodeIsUsed(OP_MTLO))
      this->using_lo = true;

    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs );
    emit->bc_pushregister( this->rt );
    emit->bc_invokestatic( "%sCRunTime/%s(II)J",
        controller->getJasminPackagePath(), this->bc);
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

  size_t getMaxStackHeight()
  {
    return 6;
  }
protected:
  bool using_hi, using_lo;
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
    if (this->using_lo && this->using_hi == false)
      {
        /* Just calculate the low parts */
        emit->bc_pushregister( this->rs );
        emit->bc_pushregister( this->rt );
        emit->bc_imul();
        emit->bc_popregister( R_LO );
        return true;
      }
    /* hi is used, we need to generate both */
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

  int fillDestinations(int *p)
  {
    if (this->using_lo == false && this->using_hi == false)
      emit->warning("Neither hi and lo are used for mult at 0x%x\n",
                    this->getAddress());

    if (this->using_lo && this->using_hi == false)
      return this->addToRegisterUsage(R_LO, p);

    return this->addToRegisterUsage(R_LO, p) + this->addToRegisterUsage(R_HI, p);
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
    if (this->using_lo == true && this->using_hi == false)
      {
        /* Only div */
        emit->bc_pushregister( this->rs );
        emit->bc_pushregister( this->rt );
        emit->bc_idiv();
        emit->bc_popregister( R_LO );
        return true;
      }
    if (this->using_lo == false && this->using_hi == true)
      {
        /* Only mod */
        emit->bc_pushregister( this->rs );
        emit->bc_pushregister( this->rt );
        emit->bc_irem();
        emit->bc_popregister( R_HI );
        return true;
      }
    emit->bc_pushregister( this->rs );
    emit->bc_pushregister( this->rt );
    emit->bc_dup2( );
    emit->bc_idiv( );
    emit->bc_popregister( R_LO );
    emit->bc_irem( );
    emit->bc_popregister( R_HI );
    return true;
  }

  int fillDestinations(int *p)
  {
    int out = 0;

    if (this->using_lo == false && this->using_hi == false)
      emit->warning("Neither hi and lo are used for div at 0x%x\n",
                    this->getAddress());

    if (this->using_lo)
      out += this->addToRegisterUsage(R_LO, p);
    if (this->using_hi)
      out += this->addToRegisterUsage(R_HI, p);

    return out;
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
