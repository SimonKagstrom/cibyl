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

class ExceptionBuiltinBase : public Builtin
{
public:
  ExceptionBuiltinBase(const char *name) : Builtin(name)
  {
  }

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

class ExceptionBuiltinTry : public ExceptionBuiltinBase
{
public:
  ExceptionBuiltinTry() : ExceptionBuiltinBase("__NOPH_try")
  {
  }

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

    controller->pushTryStack(insn);

    return true;
  }
};
