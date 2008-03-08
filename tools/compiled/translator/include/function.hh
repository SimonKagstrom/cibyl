/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      function.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Function defs
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __FUNCTION_HH__
#define __FUNCTION_HH__

#include <mips.hh>
#include <basicblock.hh>

class JavaMethod;

class Function : public CodeBlock
{
public:
  Function(const char *name, Instruction **insns,
	   int first, int last);

  virtual ~Function();

  virtual bool pass1();

  bool pass2();

  const char *getName() { return this->name; }

  int fillDestinations(int *p);

  int fillSources(int *p);

  bool hasRegisterIndirectJumps()
  {
    return this->registerIndirectJumps;
  }

  JavaMethod *parent;
protected:
  int registerDestinations[N_REGS];
  int registerSources[N_REGS];
  int n_bbs;
  BasicBlock **bbs;
  char *name;
  bool registerIndirectJumps;
};

class StartFunction : public Function
{
public:
  StartFunction(const char *name, Instruction **insns,
                int first, int last);

  bool pass1();
};

#endif /* !__FUNCTION_HH__ */
