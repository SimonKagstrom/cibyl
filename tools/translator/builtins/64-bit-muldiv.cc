/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      exceptions.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Impl. of exception builtins
 *
 * $Id:$
 *
 ********************************************************************/
#include <javamethod.hh>
#include <controller.hh>
#include <builtins.hh>
#include <emit.hh>

extern Instruction *tryInstruction;

class MulDivBuiltinBase : public Builtin
{
public:
  MulDivBuiltinBase(const char *name, const char *bc) : Builtin(name)
  {
    this->bc = bc;
  }

  bool pass1(Instruction *insn)
  {
    return true;
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(R_A0, p) + this->addToRegisterUsage(R_A1, p) +
      this->addToRegisterUsage(R_A2, p) + this->addToRegisterUsage(R_A3, p);
  };

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_V0, p) + this->addToRegisterUsage(R_V1, p);
  };

  bool pass2(Instruction *insn)
  {
    /* Concatenate the first and second parameters */
    this->push_64_bit_from_32_bit_regs(R_A1, R_A0);
    this->push_64_bit_from_32_bit_regs(R_A3, R_A2);

    /* The actual operation */
    emit->bc_generic_insn(this->bc);

    /* Split the result to V0/V1 */
    emit->bc_dup2();
    emit->bc_pushconst(32);
    emit->bc_lushr();
    emit->bc_l2i();
    emit->bc_popregister(R_V0);

    emit->bc_l2i();
    emit->bc_popregister(R_V1);
    
    return true;
  }
  
protected:
  void push_64_bit_from_32_bit_regs(MIPS_register_t r1, MIPS_register_t r2)
  {
    emit->bc_pushregister(r1);
    emit->bc_i2l();
    emit->bc_pushregister(r2);
    emit->bc_i2l();
    emit->bc_pushconst(32);
    emit->bc_lshl();
    emit->bc_lor();
  }
  
  const char *bc;
};

class DivBuiltin : public MulDivBuiltinBase
{
public:
  DivBuiltin(const char *name) : MulDivBuiltinBase(name, "ldiv")
  {
  }  
};

class ModBuiltin : public MulDivBuiltinBase
{
public:
  ModBuiltin(const char *name) : MulDivBuiltinBase(name, "lrem")
  {
  }  
};

class ShiftBuiltinBase : public MulDivBuiltinBase
{
public:
  ShiftBuiltinBase(const char *name, const char *bc) : MulDivBuiltinBase(name, bc)
  {
  }  

  bool pass2(Instruction *insn)
  {
    /* Concatenate the first and second parameters */
    this->push_64_bit_from_32_bit_regs(R_A1, R_A0);
    emit->bc_pushregister(R_A2);

    /* The actual operation */
    emit->bc_generic_insn(this->bc);

    /* Split the result to V0/V1 */
    emit->bc_dup2();
    emit->bc_pushconst(32);
    emit->bc_lushr();
    emit->bc_l2i();
    emit->bc_popregister(R_V0);

    emit->bc_l2i();
    emit->bc_popregister(R_V1);
    
    return true;
  }
};

class ShrBuiltin : public ShiftBuiltinBase
{
public:
  ShrBuiltin(const char *name) : ShiftBuiltinBase(name, "lshr")
  {
  }  
};

class ShlBuiltin : public ShiftBuiltinBase
{
public:
  ShlBuiltin(const char *name) : ShiftBuiltinBase(name, "lshl")
  {
  }
};
