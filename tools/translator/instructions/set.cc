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
	emit->bc_invokestatic( "%sCRunTime/%s(II)I",
	    controller->getJasminPackagePath(), this->bc );
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
    emit->bc_pushregister( this->rt );
    emit->bc_i2l();
    emit->bc_pushregister( this->rs );
    emit->bc_i2l();
    emit->bc_lcmp();		// rt<rs -> -1, rt==rs -> 0, rt>rs -> 1
    emit->bc_pushconst(1);
    emit->bc_iadd();
    emit->bc_pushconst(1);	      
    emit->bc_ishr();
    emit->bc_popregister( this->rd );
    return true;
  }
};

class Sltu : public TwoRegisterSetInstruction
{
public:
  Sltu(uint32_t address, int opcode,
      MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd) : TwoRegisterSetInstruction("", address, opcode, rs, rt, rd)
  {
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rt );
    emit->bc_i2l();
    emit->bc_pushconst_l(0xFFFFFFFF);
    emit->bc_land();	
    emit->bc_pushregister( this->rs );
    emit->bc_i2l();
    emit->bc_pushconst_l(0xFFFFFFFF);
    emit->bc_land();
    emit->bc_lcmp();		// rt<rs -> -1, rt==rs -> 0, rt>rs -> 1
    emit->bc_pushconst(1);
    emit->bc_iadd();
    emit->bc_pushconst(1);	      
    emit->bc_ishr();
    emit->bc_popregister( this->rd );
    return true;
  }

  size_t getMaxStackHeight()
  {
    return 6;
  }
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
    emit->bc_invokestatic("%sCRunTime/%s(II)I",
        controller->getJasminPackagePath(), this->bc);
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

class Slti : public OneRegisterSetInstruction
{
public:
  Slti(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : OneRegisterSetInstruction("", address, opcode, rs, rt, extra)
  {
  }

  bool pass2()
  {
    emit->bc_pushconst( this->extra );
    emit->bc_i2l();
    emit->bc_pushregister( this->rs );
    emit->bc_i2l();
    emit->bc_lcmp();		// rt<rs -> -1, rt==rs -> 0, rt>rs -> 1
    emit->bc_pushconst(1);
    emit->bc_iadd();
    emit->bc_pushconst(1);	      
    emit->bc_ishr();
    emit->bc_popregister( this->rt );
    return true;
  }
};

class Sltiu : public OneRegisterSetInstruction
{
public:
  Sltiu(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : OneRegisterSetInstruction("", address, opcode, rs, rt, extra)
  {
  }

  bool pass2()
  {
    emit->bc_pushconst_l( (uint32_t)extra );
    emit->bc_pushregister( this->rs );
    emit->bc_i2l();
    emit->bc_pushconst_l(0xFFFFFFFF);
    emit->bc_land();	
    emit->bc_lcmp();		// rt<rs -> -1, rt==rs -> 0, rt>rs -> 1
    emit->bc_pushconst(1);
    emit->bc_iadd();
    emit->bc_pushconst(1);	      
    emit->bc_ishr();
    emit->bc_popregister( this->rt );
    return true;
  }

  size_t getMaxStackHeight()
  {
    return 6;
  }
};
