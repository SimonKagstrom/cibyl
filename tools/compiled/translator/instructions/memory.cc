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
    /* Either a call to a subroutine or a regular function call */
    if ( config->optimizePartialMemoryOps &&
         this->opcode != OP_LWL && this->opcode != OP_LWR)
      emit->bc_jsr("__CIBYL_memoryRead%s", this->bc);
    else
      emit->bc_invokestatic("CRunTime/memoryRead%s(I)I", this->bc);
    emit->bc_popregister( this->rt );
    return true;
  }

  int fillDestinations(int *p)
  {
    if ( config->optimizePartialMemoryOps )
      return this->addToRegisterUsage(this->rt, p) + this->addToRegisterUsage(R_MADR, p);

    return this->addToRegisterUsage(this->rt, p);
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(this->rs, p) + this->addToRegisterUsage(R_MEM, p);
  };

  virtual size_t bytecodeSize(void)
  {
    return 13;
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

class Lw : public LoadXX
{
public:
  Lw(uint32_t address, int opcode,
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

  virtual size_t bytecodeSize(void)
  {
    return 11;
  };
};

class PartialLoad : public LoadXX
{
protected:
  PartialLoad(int word_size, bool is_signed,  const char *what, uint32_t address, int opcode,
              MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : LoadXX(what, address, opcode, rs, rt, extra)
  {
    this->word_size = word_size;
    this->is_signed = is_signed;

    panic_if(this->word_size != 8 && this->word_size != 16,
             "Emitting load of invalid length %d\n", this->word_size);
  }

  bool pass2()
  {
    JavaMethod *mt = controller->getMethodByAddress(this->getAddress());

    panic_if(!mt,
             "No method for instruction at 0x%x\n",
             this->getAddress());

    if ( config->optimizePartialMemoryOps ||
         !config->optimizeInlines)
      return LoadXX::pass2();
    if (mt->getBytecodeSize() > 32768)
      {
        static JavaMethod *warn_method = NULL;

        /* Bytecode size too large to allow for direct inlining,
         * reverting to normal */
        if (warn_method != mt)
          emit->warning("Bytecode size for %s (%d) too large for inlining of lb/lh",
                        mt->getName(), mt->getBytecodeSize());
        warn_method = mt;
        return LoadXX::pass2();
      }

    int b_v = 3; /* Assume lb(u) */
    unsigned int mask_val = 0xff;

    if (this->word_size == 16)
      {
        b_v = 2;
        mask_val = 0xffff;
      }
    /* Maybe skip ra */
    if (this->rt == R_RA)
      return true;
    if (this->prefix)
      this->prefix->pass2();

    emit->bc_pushregister(R_MEM);
    emit->bc_pushindex(this->rs, this->extra);
    emit->bc_iaload();

    /* b = 3 - (address & 3) */
    emit->bc_pushconst(b_v);
    emit->bc_pushregister(this->rs);
    if (this->extra != 0)
      {
        emit->bc_pushconst(extra);
        emit->bc_iadd();
      }
    emit->bc_pushconst(b_v);
    emit->bc_iand();
    emit->bc_isub();

    /* b = b * 8 */
    emit->bc_pushconst(3);
    emit->bc_ishl();

    /* out = (val >>> b) & 0xff */
    emit->bc_iushr();
    if (this->word_size == 8 && this->is_signed) /* lb */
      emit->bc_i2b();
    else if (this->word_size == 16 && !this->is_signed) /* lhu */
      emit->bc_i2c();
    else if (this->word_size == 16 && this->is_signed) /* lh */
      emit->bc_i2s();
    else /* lbu */
      {
        emit->bc_pushconst(mask_val);
        emit->bc_iand();
      }
    emit->bc_popregister( this->rt );

    return true;
  }

  virtual size_t bytecodeSize(void)
  {
    return 24;
  };
protected:
  int word_size;
  bool is_signed;
};


class Lbu : public PartialLoad
{
public:
  Lbu(uint32_t address, int opcode,
      MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : PartialLoad(8, false, "ByteUnsigned", address, opcode, rs, rt, extra)
  {
  }
};

class Lb : public PartialLoad
{
public:
  Lb(uint32_t address, int opcode,
     MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : PartialLoad(8, true, "Short", address, opcode, rs, rt, extra)
  {
  }
};

class Lhu : public PartialLoad
{
public:
  Lhu(uint32_t address, int opcode,
      MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : PartialLoad(16, false, "ShortUnsigned", address, opcode, rs, rt, extra)
  {
  }
};

class Lh : public PartialLoad
{
public:
  Lh(uint32_t address, int opcode,
     MIPS_register_t rs, MIPS_register_t rt, int32_t extra) : PartialLoad(16, true, "Short", address, opcode, rs, rt, extra)
  {
  }
};


class Sw : public StoreXX
{
public:
  Sw(uint32_t address, int opcode,
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

  virtual size_t bytecodeSize(void)
  {
    return 11;
  };
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
