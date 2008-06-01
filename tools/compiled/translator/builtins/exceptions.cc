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

    tryInstruction = insn;

    return true;
  }
};

class ExceptionBuiltinCatch : public ExceptionBuiltinBase
{
public:
  ExceptionBuiltinCatch() : ExceptionBuiltinBase("__NOPH_catch")
  {
  }

  int fillSources(int *p)
  {
    return this->addToRegisterUsage(R_SP, p); /* ECB, EAR in try */
  };

  bool pass2(Instruction *insn)
  {
    JavaMethod *method = controller->getMethodByAddress(insn->getAddress());
    uint32_t start = tryInstruction->getAddress();
    uint32_t end = insn->getAddress();
    const char *handler = method->addExceptionHandler(start, end);

    emit->generic(".catch all from L_%x to L_%x using %s\n",
                     start, end, handler);
    return true;
  }
};
