/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      jfmt.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Jfmt instructions (jumps and calls)
 *
 * $Id:$
 *
 ********************************************************************/

class BranchInstruction : public Instruction
{
public:
  BranchInstruction(uint32_t address, int opcode,
		    MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd,
		    int32_t extra) : Instruction(address, opcode, rs, rt, rd, extra)
  {
  }

  bool pass1()
  {
    if (this->delayed)
      this->delayed->pass1();
    return true;
  }

  bool isBranch()
  {
    return true;
  }

  size_t getMaxStackHeight()
  {
    return 3 + this->delayed ? this->delayed->getMaxStackHeight() : 0;
  }
};

class Jump : public BranchInstruction
{
public:
  Jump(uint32_t address, int opcode, int32_t extra) : BranchInstruction(address, opcode, R_ZERO, R_ZERO, R_ZERO, extra)
  {
  }

  bool pass1()
  {
    Instruction *dstInsn = controller->getBranchTarget(this->extra << 2);

    if (this->delayed)
      this->delayed->pass1();

    dstInsn->setBranchTarget();

    return true;
  }

  bool pass2()
  {
    Instruction *dst = controller->getBranchTarget(this->extra << 2);

    if (this->delayed)
      this->delayed->pass2();

    if (!dst)
      {
	emit->error("Jump from 0x%x to 0x%x: target not found\n",
		    this->address, this->extra << 2);
	return false;
      }

    emit->bc_goto(dst->getAddress());

    return true;
  }
};


class Jal : public BranchInstruction
{
public:
  Jal(uint32_t address, int opcode, int32_t extra) : BranchInstruction(address, opcode, R_ZERO, R_ZERO, R_ZERO, extra)
  {
    this->method = NULL;
    this->dstMethod = NULL;
    this->dstClass = NULL;
    this->builtin = NULL;
  }

  Jal(uint32_t address, int opcode, MIPS_register_t rs, int32_t extra) : BranchInstruction(address, opcode, rs, R_ZERO, R_ZERO, extra)
  {
    this->method = NULL;
    this->dstMethod = NULL;
    this->dstClass = NULL;
    this->builtin = NULL;
  }

  bool pass1()
  {
    uint32_t dst = this->extra << 2;

    this->method = controller->getMethodByAddress(this->getAddress());
    panic_if(!this->method, "No method found for jal at 0x%x\n", this->getAddress());

    this->dstMethod = controller->getMethodByAddress(dst);
    panic_if(!this->dstMethod, "No method found for jal to 0x%x\n", dst);

    this->dstClass = controller->getClassByMethodName(this->dstMethod->getName());
    panic_if(!this->dstClass, "No class found for method %s\n",
             this->dstMethod->getName());

    if (this->delayed)
      this->delayed->pass1();

    if (!this->dstMethod)
      {
	emit->error("Jal from 0x%x to 0x%x: Target address not found\n",
		    this->address, dst);
	return false;
      }

    this->builtin = controller->matchBuiltin(this, this->dstMethod->getName());
    if (this->builtin)
      return this->builtin->pass1(this);

    return true;
  }

  virtual bool pass2()
  {
    void *it;
    uint32_t dst = this->extra << 2;

    if (this->delayed)
      this->delayed->pass2();
    if (this->builtin)
      return this->builtin->pass2(this);

    /* OK, a bit ugly... */
    if (this->opcode == OP_JAL && this->method->hasMultipleFunctions() &&
        this->method == this->dstMethod)
      {
        int handle = this->method->addReturnLocation(this->getAddress() + 8);

        panic_if(handle < 0,
                 "Could not add return address at 0x%x: %d\n",
                 this->getAddress() + 8, handle);

        emit->bc_pushconst(handle);
        emit->bc_popregister(R_RA);
        emit->bc_goto(dst);

        return true;
      }

    for (MIPS_register_t reg = this->dstMethod->getFirstRegisterToPass(&it);
	 reg != 0;
	 reg = this->dstMethod->getNextRegisterToPass(&it))
      {
        /* Push the function index if it has one*/
        if (reg == R_FNA)
          {
            int idx = this->dstMethod->getFunctionIndexByAddress(dst);

            panic_if(idx < 0, "Method %s has no function for address 0x%x\n",
                this->dstMethod->getName(), dst);
            emit->bc_pushconst(idx);
          }
        else
          emit->bc_pushregister( reg );
      }

    emit->bc_invokestatic("%s%s/%s",
        controller->getJasminPackagePath(), dstClass->getName(), this->dstMethod->getJavaMethodName());

    if (config->threadSafe)
      {
        if (this->dstMethod->returnSize() == 2)
          {
            /* We have a 64-bit value on the stack */
            emit->bc_dup2();
            emit->bc_pushconst(32);
            emit->bc_lushr();
            emit->bc_l2i(); /* v1 */
            emit->bc_popregister( R_V1 );
            emit->bc_l2i(); /* v0 */
            emit->bc_popregister( R_V0 );
          }
        else if (this->dstMethod->returnSize() == 1)
          emit->bc_popregister( R_V0 );
        /* else: Nada */
      }
    else
      {
        if (this->dstMethod->returnSize() == 2)
          {
            emit->bc_getstatic("%sCRunTime/saved_v1 I",
                controller->getJasminPackagePath());
            emit->bc_popregister( R_V1 );
            emit->bc_popregister( R_V0 );
          }
        else if (this->dstMethod->returnSize() == 1)
          emit->bc_popregister( R_V0 );
      }

    return true;
  }

  int fillDestinations(int *p)
  {
    int out = 0;

    out += this->addToRegisterUsage(R_V0, p) + this->addToRegisterUsage(R_V1, p);
    if (this->builtin)
      out += this->builtin->fillDestinations(p);
    if (this->method == this->dstMethod && this->method->hasMultipleFunctions())
      out += this->addToRegisterUsage(R_RA, p);

    return out;
  }

  int fillSources(int *p)
  {
    int out = 0;

    out += this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(R_A0, p) +
      this->addToRegisterUsage(R_A1, p) + this->addToRegisterUsage(R_A2, p) +
      this->addToRegisterUsage(R_A3, p) + this->addToRegisterUsage(R_SP, p);
    if (this->builtin)
      out += this->builtin->fillSources(p);

    return out;
  };

  size_t getMaxStackHeight()
  {
    /* Cowardly assume all 6 possible registers are passed */
    return 6;
  }

protected:
  JavaMethod *method;
  JavaMethod *dstMethod;
  JavaClass *dstClass;
  Builtin *builtin;
};

class Jalr : public Jal
{
public:
  Jalr(uint32_t address, int opcode, MIPS_register_t rs) : Jal(address, opcode, rs, 0)
  {
  }

  bool pass1()
  {
    this->dstMethod = controller->getCallTableMethod();
    panic_if(!this->dstMethod, "No method found for jalr to 0x%x\n", this->extra << 2);

    this->dstClass = controller->getClassByMethodName(this->dstMethod->getName());
    panic_if(!this->dstClass, "No class found for method %s\n",
             this->dstMethod->getName());

    if (this->delayed)
      this->delayed->pass1();

    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister(this->rs);

    return Jal::pass2();
  }
};


class JalReturn : public Jal
{
public:
  JalReturn(uint32_t address, int opcode, int32_t extra) : Jal(address, opcode, extra)
  {
  }

  bool pass2()
  {
    bool out = Jal::pass2();

    /* Return from this function (undo the tail call optimization) */
    emit->bc_goto("__CIBYL_function_return");

    return out;
  }
};


class Jr : public BranchInstruction
{
public:
  Jr(uint32_t address, int opcode, MIPS_register_t rs) : BranchInstruction(address, opcode,
      rs, R_ZERO, R_ZERO, 0)
  {
  }

  virtual bool isRegisterIndirectJump()
  {
    if ( this->rs != R_RA )
      return true;
    return false;
  }

  virtual bool isReturnJump()
  {
    if ( this->rs == R_RA )
      return true;
    return false;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p);
  };

  bool pass2()
  {
    if (this->rs == R_RA)
      {
	if (this->delayed)
	  this->delayed->pass2();
	emit->bc_goto("__CIBYL_function_return");
      }
    else
      {
	emit->bc_pushregister( this->rs );
	if (this->delayed)
	  this->delayed->pass2();
	emit->bc_goto("__CIBYL_local_jumptab");
      }

    return true;
  }
};


class TwoRegisterConditionalJump : public BranchInstruction
{
public:
  TwoRegisterConditionalJump(const char *what, uint32_t address, int opcode, MIPS_register_t rs,
			     MIPS_register_t rt, int32_t extra) : BranchInstruction(address, opcode, rs, rt, R_ZERO, extra)
  {
    this->bc = what;
    this->dst = (this->address + 4) + (this->extra << 2);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(this->rt, p);
  };

  bool pass1()
  {
    Instruction *dstInsn = controller->getBranchTarget(this->dst);
    JavaMethod *srcMethod = controller->getMethodByAddress( this->getAddress() );
    JavaMethod *dstMethod = controller->getMethodByAddress( dstInsn->getAddress() );

    if (this->delayed)
      this->delayed->pass1();

    if ( !dstInsn )
      {
	emit->error("The branch at 0x%x in does not have a target (address 0x%x)\n",
		    this->getAddress(), this->dst);
      }

    if ( srcMethod != dstMethod)
      {
	emit->error("The branch at 0x%x in method %s ends in method %s\n",
		    this->getAddress(), srcMethod->getName(), dstMethod->getName());
	return false;
      }
    dstInsn->setBranchTarget();

    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister( this->rs );
    emit->bc_pushregister( this->rt );

    if (this->delayed)
      this->delayed->pass2();
    emit->bc_condbranch("%s L_%x", this->bc, this->dst);

    return true;
  }

  size_t getMaxStackHeight()
  {
    return 4 + this->delayed ? this->delayed->getMaxStackHeight() : 0;
  }

private:
  const char *bc;
  uint32_t dst;
};


class OneRegisterConditionalJump : public BranchInstruction
{
public:
  OneRegisterConditionalJump(const char *what, uint32_t address, int opcode, MIPS_register_t rs,
			     int32_t extra) : BranchInstruction(address, opcode, rs, R_ZERO, R_ZERO, extra)
  {
    this->bc = what;
    this->dst = (this->address + 4) + (this->extra << 2);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p);
  };

  bool pass1()
  {
    Instruction *dstInsn = controller->getBranchTarget(this->dst);
    JavaMethod *srcMethod = controller->getMethodByAddress( this->getAddress() );
    JavaMethod *dstMethod = controller->getMethodByAddress( dstInsn->getAddress() );

    if (this->delayed)
      this->delayed->pass1();

    if ( !dstInsn )
      {
	emit->error("The branch at 0x%x in does not have a target (address 0x%x)\n",
		    this->getAddress(), this->dst);
      }

    if ( srcMethod != dstMethod)
      {
	emit->error("The branch at 0x%x in method %s ends in method %s\n",
		    this->getAddress(), srcMethod->getName(), dstMethod->getName());
	return false;
      }
    dstInsn->setBranchTarget();

    return true;
  }

  bool pass2()
  {

    emit->bc_pushregister( this->rs );

    if (this->delayed)
      this->delayed->pass2();
    emit->bc_condbranch("%s L_%x", this->bc, this->dst);

    return true;
  }

  size_t getMaxStackHeight()
  {
    return 3 + this->delayed ? this->delayed->getMaxStackHeight() : 0;
  }

private:
  const char *bc;
  uint32_t dst;
};
