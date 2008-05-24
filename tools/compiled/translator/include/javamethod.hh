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

#include <ght_hash_table.h>

#include <function.hh>
#include <mips.hh>
#include <utils.h>
#include <elf.hh>
#include <registerallocator.hh>

class JavaClass;

/* Helper class */
class ExceptionHandler
{
public:
  ExceptionHandler(uint32_t start, uint32_t end)
  {
    this->start = start;
    this->end = end;

    this->name = (char*)xcalloc(32, 1);
    snprintf(this->name, 32, "L_EXH_%08x_%08x", start, end);
  }

  uint32_t start;
  uint32_t end;
  char *name;
};

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

  /**
   * Add an exception handler between @start and @end
   *
   * @param start the start address
   * @param end the end address
   *
   * @return the name of the exception handler
   */
  char *addExceptionHandler(uint32_t start, uint32_t end);

protected:
  Function **functions;
  int n_functions;
  int registerUsage[N_REGS];

  char *javaName;

  int n_registersToPass;
  bool registerIndirectJumps;

  int n_exceptionHandlers;
  ExceptionHandler **exceptionHandlers;
};

class CallTableMethod : public JavaMethod
{
public:
  CallTableMethod(int maxMethods, cibyl_exported_symbol_t *exp_syms, size_t n_exp_syms);

  void addMethod(JavaMethod *method);

  bool pass1();

  bool pass2();

  const char *getName()
  {
    return "call";
  }

  char *getJavaMethodName()
  {
    return (char*)"CibylCallTable/call(IIIIII)I";
  }
protected:
  JavaMethod **methods;
  cibyl_exported_symbol_t *exp_syms;
  ght_hash_table_t *method_table;
  size_t n_exp_syms;
  int n_methods;
};

#endif /* !__JAVAMETHOD_HH__ */
