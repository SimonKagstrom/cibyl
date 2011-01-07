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

#include <map>

#include <function.hh>
#include <mips.hh>
#include <utils.h>
#include <elf.hh>
#include <config.hh>
#include <registerallocator.hh>

using namespace std;

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
  
  virtual bool pass2();

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

  bool registerIsArgument(MIPS_register_t reg);

  /**
   * The size of the return value, in words.
   *
   * @return 0, 1 or 2.
   */
  int returnSize();

  void setReturnSize(int n);

  int getRegistersToPass()
  {
    return this->n_registersToPass;
  }

  bool opcodeIsUsed(mips_opcode_t op)
  {
    for (int i = 0; i < this->n_functions; i++)
      if (this->functions[i]->opcodeIsUsed(op))
        return true;
    return false;
  }

  /**
   * Add an exception handler between @start and @end
   *
   * @param eh the exception handler to add
   *
   * @return the name of the exception handler
   */
  char *addExceptionHandler(ExceptionHandler *eh);

  /**
   * Add a return address to this method (for multi-function methods)
   *
   * @param address the address to return to
   * @return an integer with a handle of the return address (starting at 0)
   */
  int addReturnLocation(uint32_t address);

  /**
   * Returns if this method is a multi-function-one
   */
  virtual bool hasMultipleFunctions()
  {
    return (this->n_functions > 1);
  }

  int getNumberOfFunctions()
  {
    return this->n_functions;
  }

  virtual size_t getBytecodeSize(void)
  {
    return this->bc_size;
  };

  virtual size_t getMaxStackHeight(void)
  {
    return this->maxStackHeight;
  };

  Function *getFunctionByAddress(uint32_t addr);

  int getFunctionIndexByAddress(uint32_t addr);

protected:
  void emitLoadSubroutine(mips_opcode_t op);
  void emitStoreSubroutine(mips_opcode_t op);
  void emitSubroutineForOp(mips_opcode_t op);
  void emitSubroutines();

  Function **functions;
  int n_functions;
  int registerUsage[N_REGS];

  char *javaName;

  int n_registersToPass;
  bool registerIndirectJumps;

  int n_exceptionHandlers;
  ExceptionHandler **exceptionHandlers;

  int n_returnLocations;
  uint32_t *returnLocations;

  size_t bc_size;
  size_t maxStackHeight;

  int m_returnSize;

  /* M_ZERO-terminated list */
  MIPS_register_t *m_possibleArguments;
};

class CallTableMethod : public JavaMethod
{
public:
  CallTableMethod(int maxFunctions, cibyl_exported_symbol_t *exp_syms, size_t n_exp_syms);

  void addFunction(Function *fn);

  bool pass1();

  bool pass2();

  const char *getName()
  {
    return "call";
  }

  char *getJavaMethodName()
  {
    if (config->threadSafe)
      return (char*)"call(IIIIII)J";
    return (char*)"call(IIIIII)I";
  }

  bool hasMultipleFunctions()
  {
    return false;
  }

  typedef map<uint32_t, Function *> JavaFunctionTable_t;

protected:
  /* Generate a method with name @a name, with methods from @a start
   * to @a end */
  void generateMethod(const char *name, int start, int end);

  /* Generate a hierarchy of methods */
  void generateHierarchy(unsigned int n);

  Function **functions;
  cibyl_exported_symbol_t *exp_syms;
  JavaFunctionTable_t m_function_table;
  size_t n_exp_syms;
  int n_function;
};

#endif /* !__JAVAMETHOD_HH__ */
