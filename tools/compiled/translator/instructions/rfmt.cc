/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      rfmt.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Rfmt instructions
 *
 * $Id:$
 *
 ********************************************************************/
class Rfmt : public Instruction
{
public:
  Rfmt(const char *what, uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd) : Instruction(address, opcode, rs, rt, rd, 0)
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
    emit->bc_pushregister( this->rt );
    emit->bc_generic_insn( this->bc );
    emit->bc_popregister( this->rd );

    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rd, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(this->rt, p);
  };

protected:
  const char *bc;
};


class Addu : public Rfmt
{
public:
  Addu(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd) : Rfmt("iadd", address, opcode, rs, rt, rd)
  {
  }

  bool pass2()
  {
    /* Nop */
    if (this->rt == 0 && this->rs == 0 && this->rd == 0)
      return true;
    if (this->rt != 0 && this->rs != 0)
      return Rfmt::pass2();

    if (this->rt == 0)
	emit->bc_pushregister(this->rs);
    else if (this->rs == 0)
	emit->bc_pushregister(this->rt);
    else
      panic("Neither rt nor rs is 0 (should have been catched earlier))\n");
    emit->bc_popregister(this->rd);

    return true;
  }
};

class Subu : public Rfmt
{
public:
  Subu(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd) : Rfmt("isub", address, opcode, rs, rt, rd)
  {
  }

  bool pass2()
  {
    if (this->rs == R_ZERO)
      {
	emit->bc_pushregister( this->rt );
	emit->bc_ineg();
	emit->bc_popregister( this->rd );
      }
    else
      return Rfmt::pass2();

    return true;
  }
};

class Nor : public Rfmt
{
public:
  Nor(uint32_t address, int opcode,
      MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd) : Rfmt("", address, opcode, rs, rt, rd)
  {
  }

  bool pass2()
  {
    if ( this->rs != R_ZERO || this->rt != R_ZERO )
      {
	if ( this->rs != R_ZERO && this->rt != R_ZERO )
	  {
	    emit->bc_pushregister( this->rs );
	    emit->bc_pushregister( this->rt );
	    emit->bc_ior();
	  }
	else if ( this->rs != R_ZERO )
	  emit->bc_pushregister( this->rs );
	else
	  emit->bc_pushregister( this->rt );
	emit->bc_pushconst(-1);
	emit->bc_ixor();
      }
    else
      emit->bc_pushconst(-1);
    emit->bc_popregister( this->rd );

    return true;
  }
};

class ShiftInstruction : public Rfmt
{
public:
  ShiftInstruction(const char *what, uint32_t address, int opcode,
		   MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd, int32_t extra) : Rfmt(what, address, opcode, rs, rt, rd)
  {
    this->extra = extra;
  }

  bool pass2()
  {
    /* Nop */
    if (this->rt == 0 && this->extra == 0 && this->rd == 0)
      return true;

    emit->bc_pushregister( this->rt );
    emit->bc_pushconst( this->extra );
    emit->bc_generic_insn( this->bc );
    emit->bc_popregister( this->rd );

    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rd, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rt, p);
  };

};


class ShiftInstructionV : public Rfmt
{
public:
  ShiftInstructionV(const char *what, uint32_t address, int opcode,
		    MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd) : Rfmt(what, address, opcode, rs, rt, rd)
  {
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rt );
    emit->bc_pushregister( this->rs );
    emit->bc_generic_insn( this->bc );
    emit->bc_popregister( this->rd );

    return true;
  }
};
