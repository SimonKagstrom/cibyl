/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      set.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Set instructions
 *
 * $Id:$
 *
 ********************************************************************/

class TwoRegisterSetInstruction : public Rfmt
{
public:
  TwoRegisterSetInstruction(const char *what, uint32_t address, int opcode,
			    MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd) : Rfmt(what, address, opcode, rs, rt, rd)
  {
  }

  bool pass2()
  {
    if (this->rs == R_ZERO)
      {
	emit->bc_pushconst(1);
	emit->bc_pushregister( this->rt );
	emit->bc_condbranch("ifne L_tmp_%x", this->address);
	emit->bc_pop();
	emit->bc_pushconst(0);
	emit->bc_label("L_tmp_%x", this->address);
      }
    else
      {
	emit->bc_pushregister( this->rs );
	emit->bc_pushregister( this->rt );
	emit->bc_invokestatic( "CRunTime/%s(II)I", this->bc );
      }
    emit->bc_popregister( this->rd );
    return true;
  }
};



class Slt : public TwoRegisterSetInstruction
{
public:
  Slt(uint32_t address, int opcode,
      MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd) : TwoRegisterSetInstruction("", address, opcode, rs, rt, rd)
  {
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs );
    emit->bc_pushregister( this->rt );
    emit->bc_isub();
    emit->bc_pushconst( 31 );
    emit->bc_iushr();
    emit->bc_popregister( this->rd );
    return true;
  }
};


class Slti : public OneRegisterSetInstruction
{
public:
  Slti(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : OneRegisterSetInstruction("", address, opcode, rs, rt, extra)
  {
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs );
    emit->bc_pushconst( this->extra );
    emit->bc_isub();
    emit->bc_pushconst( 31 );
    emit->bc_iushr();
    emit->bc_popregister( this->rt );
    return true;
  }
};
