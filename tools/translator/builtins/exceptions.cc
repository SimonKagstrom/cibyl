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
#include <controller.hh>

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

  bool pass1(Instruction *insn)
  {
    insn->setBranchTarget();
    controller->pushTryStack(insn);
    return true;
  }

  bool pass2(Instruction *insn)
  {
    emit->bc_pushregister(R_A0);
    emit->bc_popregister(R_ECB);
    emit->bc_pushregister(R_A1);
    emit->bc_popregister(R_EAR);

    return true;
  }
};

class SetjmpExceptionHandler : public ExceptionHandler
{
public:
  SetjmpExceptionHandler(JavaMethod *mt, uint32_t target) : 
    ExceptionHandler(mt->getAddress(), mt->getAddress() + mt->getSize())
  {
    this->mt = mt;
    this->target = target;
  }

  bool pass2()
  {
    emit->bc_label("%s", this->name);

    /* Copy the exception object ref to get the cookie and value (
     * and prepare for throwing) */
    emit->bc_dup();

    /* Push the cookie and the argument (passed to the __NOPH_setjmp
     * function) - if these are equal, this was for us - otherwise
     * just rethrow it */
    emit->bc_invokevirtual("%sSetjmpException/getCookie()I",
        controller->getJasminPackagePath());
    emit->bc_pushconst(2);
    emit->bc_ishr();
    emit->bc_pushregister(R_MEM);
    emit->bc_swap();
    emit->bc_iaload(); /* load *cookie */
    emit->bc_pushconst( this->target );
    emit->bc_if_icmpne("L_setjmp_handler_%s_not_this", this->name);

    /* Put value in V0 */
    emit->bc_invokevirtual("%sSetjmpException/getValue()I",
        controller->getJasminPackagePath());
    emit->bc_popregister(R_V0);
    
    emit->bc_goto( this->target );

    /* Wrong one - throw it */
    emit->bc_label("L_setjmp_handler_%s_not_this", this->name);
    emit->bc_athrow(); /* And throw the other again */

    return true;
  }
protected:
  JavaMethod *mt;
  uint32_t target;
};

class SetjmpBuiltin : public Builtin
{
public:
  SetjmpBuiltin(const char *name) : Builtin(name)
  {
  }

  bool pass1(Instruction *insn)
  {
    return true;
  }

  /* Register this function as a setjmp one */
  /* In this class: Add a label */
  bool pass2(Instruction *insn)
  {
    uint32_t target = insn->getAddress();

    JavaMethod *mt = controller->getMethodByAddress( target );
    const char *handler = mt->addExceptionHandler(new SetjmpExceptionHandler(mt,
        target) );
    
    /* Catch the SetjmpException in the entire method */
    emit->generic(".catch %sSetjmpException from __CIBYL_javamethod_begin to __CIBYL_exception_handlers using %s\n",
        controller->getJasminPackagePath(),
        handler);
    emit->bc_pushconst(0);
    emit->bc_popregister(R_V0);
    emit->bc_label( target );
    
    return true;
  }
};

class ThrowBuiltin : public Builtin
{
public:
  ThrowBuiltin() : Builtin("__NOPH_throw")
  {
  }

  bool pass1(Instruction *insn)
  {
    return true;
  }

  bool pass2(Instruction *insn)
  {
    emit->bc_getstatic("%sCRunTime/objectRepository [Ljava/lang/Object;",
        controller->getJasminPackagePath());
    emit->bc_pushregister(R_A0);
    emit->bc_aaload();
    emit->bc_checkcast("java/lang/Throwable");
    emit->bc_athrow(); /* Throw! */
    return true;
  }
};
