/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      javamethod.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Java method
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __JAVAMETHOD_HH__
#define __JAVAMETHOD_HH__

#include <function.hh>
#include <mips.hh>
#include <registerallocator.hh>

class JavaClass;

class JavaMethod : public CodeBlock
{
public:
  JavaMethod(Function **fns, int first, int last);

  virtual ~JavaMethod();

  virtual bool pass1();

  virtual bool pass2();

  RegisterAllocator *getRegisterAllocator();

  virtual const char *getName()
  {
    return this->functions[0]->getName();
  }

  virtual char *getJavaMethodName();

  bool clobbersReg(MIPS_register_t reg);

  /**
   * Iterator over the MIPS registers to pass to this function
   */
  MIPS_register_t getFirstRegisterToPass(void *it);

  MIPS_register_t getNextRegisterToPass(void *it);

  int getRegistersToPass()
  {
    return this->n_registersToPass;
  }

protected:
  Function **functions;
  int n_functions;
  int registerUsage[N_REGS];

  char *javaName;

  int n_registersToPass;
  bool registerIndirectJumps;
};

class CallTableMethod : public JavaMethod
{
public:
  CallTableMethod(int maxMethods);

  void addMethod(JavaMethod *method);

  bool pass1();

  bool pass2();

  const char *getName()
  {
    return "call";
  }

  char *getJavaMethodName()
  {
    return "CibylCallTable/call(IIIIII)I";
  }
protected:
  JavaMethod **methods;
  int n_methods;
};

#endif /* !__JAVAMETHOD_HH__ */
