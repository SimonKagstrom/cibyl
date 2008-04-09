/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      memory.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Memory insns
 *
 * $Id:$
 *
 ********************************************************************/

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

class LWc1 : public LoadXX
{
public:
  LWc1(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : LoadXX("", address, opcode, rs, rt, extra)
  {
    assert(rt >= R_F0 && rt <= R_F31);
  }

  bool pass2()
  {
    emit->warning("Instruction LWc1 not yet implemented\n");
    return true;
  }
};

class SWc1 : public StoreXX
{
public:
  SWc1(uint32_t address, int opcode,
       MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : StoreXX("", address, opcode, rs, rt, extra)
  {
    assert(rt >= R_F0 && rt <= R_F31);
  }

  bool pass2()
  {
    emit->warning("Instruction SWc1 not yet implemented\n");
    return true;
  }
};
