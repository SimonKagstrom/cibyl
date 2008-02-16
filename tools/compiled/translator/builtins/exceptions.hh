/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      exceptions.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Impl. of exception builtins
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __EXCEPTIONS_HH__
#define __EXCEPTIONS_HH__

#include <builtins.hh>
#include <emit.hh>

extern Instruction *tryInstruction;

class ExceptionBuiltinBase : public Builtin
{
public:
  bool pass1(Instruction *insn)
  {
    insn->setBranchTarget();
    return true;
  }

  int fillDestinations(int *p)
  {
    return this->addToRegisterUsage(R_EAR, p) + this->addToRegisterUsage(R_ECB, p);
  };
};

class ExcetpionBuiltinTry : ExceptionBuiltinBase
{
public:
  int fillSources(int *p)
  {
    return this->addToRegisterUsage(R_A0, p) + this->addToRegisterUsage(R_A1, p);
  };

  bool pass2(Instruction *insn)
  {
    emit->bc_pushregister(R_A0);
    emit->bc_popregister(R_ECB);
    emit->bc_pushregister(R_A1);
    emit->bc_popregister(R_EAR);

    tryInstruction = insn;

    return true;
  }
};

#endif /* !__EXCEPTIONS_HH__ */
