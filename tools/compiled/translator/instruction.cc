/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      instruction.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Insn implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <assert.h>
#include <stdio.h>

#include <instruction.hh>
#include <controller.hh>
#include <javaclass.hh>
#include <emit.hh>
#include <config.hh>
#include <utils.h>

#define max(x,y) ( (x) > (y) ? (x) : (y) )

Instruction::Instruction(uint32_t address, int opcode,
			 MIPS_register_t rs, MIPS_register_t rt, MIPS_register_t rd,
			 int32_t extra)
{
  this->address = address;
  this->size = 4;
  this->opcode = opcode;
  this->rs = rs;
  this->rt = rt;
  this->rd = rd;
  this->extra = extra;
  this->branchTarget = false;
  this->prefix = NULL;
  this->delayed = NULL;
};

Instruction::~Instruction()
{
}

void Instruction::setBranchTarget()
{
  this->branchTarget = true;
}

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
      assert(0 && "BUG!");
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

class LoadXX : public Instruction
{
public:
  LoadXX(const char *what, uint32_t address, int opcode,
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
    emit->bc_pushaddress( this->rs, this->extra );
    emit->bc_invokestatic("CRunTime/memoryRead%s(I)I", this->bc);
    emit->bc_popregister( this->rt );
    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(this->rt, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(R_MEM, p);
  };
protected:
  const char *bc;
};


class StoreXX : public Instruction
{
public:
  StoreXX(const char *what, uint32_t address, int opcode,
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
    if (config->traceStores)
      this->traceStore();

    emit->bc_pushaddress( this->rs, this->extra );
    emit->bc_pushregister( this->rt );
    emit->bc_invokestatic("CRunTime/memoryWrite%s(II)V", this->bc);
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(this->rt, p) + this->addToRegisterUsage(R_MEM, p);
  };
protected:

  void traceStore()
  {
    emit->bc_pushconst( this->address );
    emit->bc_pushaddress( this->rs, this->extra );
    emit->bc_pushregister( this->rt );

    emit->bc_invokestatic("CRunTime/memoryWrite%sPc(III)V", this->bc);
  }

  const char *bc;
};

class LoadXXSigned : public LoadXX
{
public:
  LoadXXSigned(const char *what, const char *convert, uint32_t address, int opcode,
	       MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : LoadXX(what, address, opcode, rs, rt, extra)
  {
    this->convert = convert;
  }

  bool pass2()
  {
    emit->bc_pushaddress( this->rs, this->extra );
    emit->bc_invokestatic("CRunTime/memoryRead%s(I)I", this->bc);
    emit->bc_generic_insn( this->convert );
    emit->bc_popregister( this->rt );
    return true;
  }
private:
  const char *convert;
};

class LW : public LoadXX
{
public:
  LW(uint32_t address, int opcode,
     MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : LoadXX("", address, opcode, rs, rt, extra)
  {
  }

  bool pass2()
  {
    /* Skip stores to RA */
    if (this->rt == R_RA)
      return true;

    if (this->prefix)
      this->prefix->pass2();
    emit->bc_pushregister( R_MEM );
    emit->bc_pushindex( this->rs, this->extra );
    emit->bc_iaload();
    emit->bc_popregister( this->rt );
    return true;
  }
};

class SW : public StoreXX
{
public:
  SW(uint32_t address, int opcode,
     MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : StoreXX("Word", address, opcode, rs, rt, extra)
  {
  }

  bool pass2()
  {
    /* Skip stores to RA */
    if (this->rt == R_RA)
      return true;

    if (config->traceStores)
      this->traceStore();

    if (this->prefix)
      this->prefix->pass2();
    emit->bc_pushregister( R_MEM );
    emit->bc_pushindex( this->rs, this->extra );
    emit->bc_pushregister( this->rt );
    emit->bc_iastore();
    return true;
  }
};

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

  int getMaxStackHeight() { return 0; }
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


class Jalr : public BranchInstruction
{
public:
  Jalr(uint32_t address, int opcode, MIPS_register_t rs) : BranchInstruction(address, opcode, rs, R_ZERO, R_ZERO, 0)
  {
    this->dstMethod = NULL;
  }

  bool pass1()
  {
    this->dstMethod = controller->getCallTableMethod();

    assert(this->dstMethod);

    if (this->delayed)
      this->delayed->pass1();

    return true;
  }

  bool pass2()
  {
    void *it;

    emit->bc_pushregister(this->rs);
    if (this->delayed)
      this->delayed->pass2();
    for (MIPS_register_t reg = this->dstMethod->getFirstRegisterToPass(&it);
	 reg != 0;
	 reg = this->dstMethod->getNextRegisterToPass(&it))
      {
	emit->bc_pushregister( reg );
      }
    emit->bc_invokestatic( "%s", this->dstMethod->getJavaMethodName() );

    if (this->dstMethod->clobbersReg(R_V1))
      {
	emit->bc_getstatic("CRunTime/saved_v1 I");
	emit->bc_popregister( R_V1 );
      }
    if (this->dstMethod->clobbersReg(R_V0))
      emit->bc_popregister( R_V0 );

    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_V0, p) + this->addToRegisterUsage(R_V1, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(R_A0, p) + this->addToRegisterUsage(R_A1, p) + this->addToRegisterUsage(R_A2, p) + this->addToRegisterUsage(R_A3, p) + this->addToRegisterUsage(R_SP, p);
  };

private:
  JavaMethod *dstMethod;
};

class Jal : public BranchInstruction
{
public:
  Jal(uint32_t address, int opcode, int32_t extra) : BranchInstruction(address, opcode, R_ZERO, R_ZERO, R_ZERO, extra)
  {
    this->dstMethod = NULL;
    this->builtin = NULL;
  }

  bool pass1()
  {
    this->dstMethod = controller->getMethodByAddress(this->extra << 2);

    if (this->delayed)
      this->delayed->pass1();

    if (!this->dstMethod)
      {
	emit->error("Jal from 0x%x to 0x%x: Target address not found\n",
		    this->address, this->extra << 2);
	return false;
      }

    this->builtin = controller->matchBuiltin(this->dstMethod->getName());
    if (this->builtin)
      return this->builtin->pass1(this);

    return true;
  }

  virtual bool pass2()
  {
    void *it;

    if (this->delayed)
      this->delayed->pass2();

    if (this->builtin)
      return this->builtin->pass2(this);

    /* Pass registers */
    for (MIPS_register_t reg = this->dstMethod->getFirstRegisterToPass(&it);
	 reg != 0;
	 reg = this->dstMethod->getNextRegisterToPass(&it))
      {
	emit->bc_pushregister( reg );
      }
    emit->bc_invokestatic("%s/%s", "Cibyl", this->dstMethod->getJavaMethodName());

    if (this->dstMethod->clobbersReg(R_V1))
      {
	emit->bc_getstatic("CRunTime/saved_v1 I");
	emit->bc_popregister( R_V1 );
      }
    if (this->dstMethod->clobbersReg(R_V0))
      emit->bc_popregister( R_V0 );

    return true;
  }

  int fillDestinations(int *p)
  {
    int out = 0;

    out += this->addToRegisterUsage(R_V0, p) + this->addToRegisterUsage(R_V1, p);
    if (this->builtin)
      out += this->builtin->fillDestinations(p);

    return out;
  }

  int fillSources(int *p)
  {
    int out = 0;

    out += this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(R_A0, p) + this->addToRegisterUsage(R_A1, p) + this->addToRegisterUsage(R_A2, p) + this->addToRegisterUsage(R_A3, p) + this->addToRegisterUsage(R_SP, p);
    if (this->builtin)
      out += this->builtin->fillSources(p);

    return out;
  };

  int getMaxStackHeight()
  {
    assert(this->dstMethod);
    return max(this->dstMethod->getRegistersToPass(), 2);
  }

protected:
  JavaMethod *dstMethod;
  Builtin *builtin;
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
  Jr(uint32_t address, int opcode, MIPS_register_t rs) : BranchInstruction(address, opcode, rs, R_ZERO, R_ZERO, 0)
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
private:
  const char *bc;
  uint32_t dst;
};



class SyscallInsn : public Instruction
{
public:
  SyscallInsn(uint32_t address, int32_t extra) : Instruction(address, 0, R_ZERO, R_ZERO, R_ZERO, extra)
  {
    this->sysc = NULL;
  }

  bool pass1()
  {
    /* FIXME: Mark syscall used */
    this->sysc = controller->getSyscall(this->extra);
    return true;
  }

  bool pass2()
  {
    emit->bc_invokestatic( this->sysc->getJavaSignature() );

    if ( this->sysc->returnsValue() )
      emit->bc_popregister( R_V0 );
    return true;
  }

  int fillDestinations(int *p)
  {
    if ( this->sysc->returnsValue() )
      return this->addToRegisterUsage(R_V0, p);
    return 0;
  };

  int getMaxStackHeight()
  {
    return this->sysc->getRegistersToPass();
  }

protected:
  Syscall *sysc;
};



class SyscallRegisterArgument : public Instruction
{
public:
  SyscallRegisterArgument(uint32_t address, int32_t extra) : Instruction(address, 0, R_ZERO, R_ZERO, R_ZERO, extra)
  {
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    emit->bc_pushregister( (MIPS_register_t)this->extra );
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage((MIPS_register_t)this->extra, p);
  };

  int getMaxStackHeight()
  {
    return 1;
  }
};


class Nop : public Instruction
{
public:
  Nop(uint32_t address) : Instruction(address, 0, R_ZERO, R_ZERO, R_ZERO, 0)
  {
  }

  virtual bool isNop()
  {
    return true;
  }

  bool pass1()
  {
    return true;
  }

  bool pass2()
  {
    return true;
  }

  int toString(char *dst, size_t n = 255)
  {
    return snprintf(dst, n, " ");
  }
};



class DelaySlotNop : public Nop
{
public:
  DelaySlotNop(uint32_t address) : Nop(address)
  {
  }

  bool isDelaySlotNop()
  {
    return true;
  }
};

/* --- The factory --- */
Instruction *InstructionFactory::create(uint32_t address, uint32_t word)
{
  MIPS_register_t rs, rt, rd;
  int32_t extra;
  mips_op_entry_t opentry;
  mips_opcode_t opcode;

  word = be32_to_host(word);

  /* Split the instruction */
  rs = mips_encoding_get_rs(word);
  rt = mips_encoding_get_rt(word);
  rd = mips_encoding_get_rd(word);

  opentry = mips_op_entries[(word >> 26) & 0x3f];
  opcode = opentry.type; /* Assume type is the opcode */

  extra = 0;

  /* Syscall */
  if ( (word >> 24) == 0xff )
    return new SyscallInsn(address, word & 0x00ffffff);

  /* Syscall argument */
  if ( (word >> 16) == 0xfefe )
    return new SyscallRegisterArgument(address, word & 0x0000ffff);

  if ( opentry.fmt == IFMT )
    {
      extra = word & 0xffff;
      if ( !mips_zero_extend_opcode(opcode) && (extra & 0x8000) == 0x8000 )
	extra = ( -1 & ~0xffff ) | extra;
    }
  else if ( opentry.fmt == RFMT )
    extra = (word >> 6) & 0x1f;
  else /* JFMT */
    extra = (word & 0x03ffffff);

  if ( opentry.type == SPECIAL )
    opcode = mips_special_table[word & 0x3f];
  else if ( opentry.type == BCOND )
    {
      switch(word & 0x1f0000)
	{
	case 0:
	  opcode = OP_BLTZ; break;
	case 0x10000:
	  opcode = OP_BGEZ; break;
	case 0x100000:
	  opcode = OP_BLTZAL; break;
	case 0x110000:
	  opcode = OP_BGEZAL; break;
	default:
	  opcode = OP_UNIMP; break;
	}
    }
  else if ( opentry.type == COP1 ) /* FPU instructions */
    {
      /* FIXME! Actually implement this */
      opcode = mips_cop1_table[rs];
    }

  /* Now we have extra, rs, rt, rd and opcode correctly setup! */
  switch(opcode)
    {
    case OP_ADDU: return new Addu(address, opcode,
				  rs, rt, rd);
    case OP_ADD: return new Rfmt("iadd", address, opcode,
				 rs, rt, rd);
    case OP_SUB: return new Rfmt("isub", address, opcode,
				 rs, rt, rd);
    case OP_SUBU: return new Subu(address, opcode,
				  rs, rt, rd);
    case OP_XOR: return new Rfmt("ixor", address, opcode,
				 rs, rt, rd);
    case OP_AND: return new Rfmt("iand", address, opcode,
				 rs, rt, rd);
    case OP_OR: return new Rfmt("ior", address, opcode,
				rs, rt, rd);
    case OP_NOR: return new Nor(address, opcode,
				rs, rt, rd);
    case OP_SRAV: return new ShiftInstructionV("ishr", address, opcode,
					       rs, rt, rd);
    case OP_SLLV: return new ShiftInstructionV("ishl", address, opcode,
					       rs, rt, rd);
    case OP_SRLV: return new ShiftInstructionV("iushr", address, opcode,
					       rs, rt, rd);
    case OP_SLL: return new ShiftInstruction("ishl", address, opcode,
					     rs, rt, rd, extra);
    case OP_SRA: return new ShiftInstruction("ishr", address, opcode,
					     rs, rt, rd, extra);
    case OP_SRL: return new ShiftInstruction("iushr", address, opcode,
					     rs, rt, rd, extra);
      /* Ifmt */
    case OP_ADDI: return new Addi(address, opcode,
				  rs, rt, rd, extra);
    case OP_ADDIU: return new Addi(address, opcode,
				   rs, rt, rd, extra);
    case OP_XORI: return new Ifmt("ixor", address, opcode,
				  rs, rt, rd, extra);
    case OP_ANDI: return new Ifmt("iand", address, opcode,
				  rs, rt, rd, extra);
    case OP_ORI: return new Ifmt("ior", address, opcode,
				 rs, rt, rd, extra);

      /* Jfmt */
    case OP_JAL: return new Jal(address, opcode, extra);
    case OP_JALR: return new Jalr(address, opcode, rs);
    case OP_J: return new Jump(address, opcode, extra);
    case OP_BGEZAL: return new Jal(address, opcode, extra); /* BAL instructions */

      /* Memory handling */
    case OP_LW: return new LW(address, opcode, rs, rt, extra);
    case OP_SW: return new SW(address, opcode, rs, rt, extra);
    case OP_LB: return new LoadXXSigned("ByteUnsigned", "i2b",
					address, opcode, rs, rt, extra);
    case OP_LBU: return new LoadXX("ByteUnsigned", address, opcode,
				   rs, rt, extra);
    case OP_LH:  return new LoadXXSigned("ShortUnsigned", "i2s",
					 address, opcode, rs, rt, extra);
    case OP_LHU: return new LoadXX("ShortUnsigned", address, opcode,
				    rs, rt, extra);
    case OP_LWL: return new LoadXX("WordLeft", address, opcode,
				   rs, rt, extra);
    case OP_LWR: return new Nop(address);
    case OP_SB: return new StoreXX("Byte", address, opcode,
				   rs, rt, extra);
    case OP_SH: return new StoreXX("Short", address, opcode,
				   rs, rt, extra);
    case OP_SWL: return new StoreXX("WordLeft", address, opcode,
				    rs, rt, extra);
    case OP_SWR: return new Nop(address);

      /* Misc other instructions */
    case OP_BREAK: return new Nop(address);
    case OP_MULT: return new Mult(address, opcode, rs, rt);
    case OP_MULTU: return new MulDiv("multu", address, opcode, rs, rt, rd, extra);
    case OP_DIV: return new Div(address, opcode, rs, rt);
    case OP_DIVU: return new MulDiv("divu", address, opcode, rs, rt, rd, extra);
    case OP_MFLO: return new Mfxx(address, opcode, rd, R_LO);
    case OP_MFHI: return new Mfxx(address, opcode, rd, R_HI);
    case OP_MTLO: return new Mtxx(address, opcode, rd, R_LO);
    case OP_MTHI: return new Mtxx(address, opcode, rd, R_HI);
    case OP_JR: return new Jr(address, opcode, rs); /* R-type, special */
    case OP_LUI: return new Lui(address, opcode, rt, extra);

      /* Conditional jumps, shifts */
    case OP_SLTU: return new TwoRegisterSetInstruction("sltu", address, opcode,
						      rs, rt, rd);
    case OP_SLT: return new Slt(address, opcode, rs, rt, rd);
    case OP_SLTIU: return new OneRegisterSetInstruction("sltu", address, opcode, rs, rt, extra);
    case OP_SLTI: return new Slti(address, opcode, rs, rt, extra);

    case OP_BEQ: return new TwoRegisterConditionalJump("if_icmpeq", address, opcode, rs, rt, extra);
    case OP_BNE: return new TwoRegisterConditionalJump("if_icmpne", address, opcode, rs, rt, extra);

    case OP_BGEZ: return new OneRegisterConditionalJump("ifge", address, opcode, rs, extra);
    case OP_BGTZ: return new OneRegisterConditionalJump("ifgt", address, opcode, rs, extra);
    case OP_BLEZ: return new OneRegisterConditionalJump("ifle", address, opcode, rs, extra);
    case OP_BLTZ: return new OneRegisterConditionalJump("iflt", address, opcode, rs, extra);

    default:
      break;
    }

  fprintf(stderr, "Pattern 0x%x on address 0x%x is not an instruction\n",
          word, address);
  exit(1);

  return NULL;
}

InstructionFactory *InstructionFactory::instance;

InstructionFactory *InstructionFactory::getInstance()
{
  if (!InstructionFactory::instance)
    InstructionFactory::instance = new InstructionFactory();

  return InstructionFactory::instance;
}

Instruction *InstructionFactory::createNop(uint32_t address)
{
  return new Nop(address);
}

Instruction *InstructionFactory::createDelaySlotNop(uint32_t address)
{
  return new DelaySlotNop(address);
}

Instruction *InstructionFactory::createTailCallJump(uint32_t address, uint32_t extra)
{
  return new JalReturn(address, OP_JAL, extra);
}
