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

  bool opcodeIsUsed(mips_opcode_t op)
  {
    panic_if(op < 0 || op > N_INSNS,
             "Opcode %d is outside the range of valid values\n", op);
    return this->usedInsns[op];
  }

  virtual size_t getBytecodeSize(void)
  {
    return this->bc_size;
  };

  JavaMethod *parent;
protected:
  void markOpcodeUsed(mips_opcode_t op)
  {
    panic_if(op < 0 || op > N_INSNS,
             "Opcode %d is outside the range of valid values\n", op);
    this->usedInsns[op] = 1;
  }

  int registerDestinations[N_REGS];
  int registerSources[N_REGS];
  int n_bbs;

  size_t bc_size;

  BasicBlock **bbs;
  char *name;
  bool registerIndirectJumps;

  uint8_t usedInsns[N_INSNS];
};

class StartFunction : public Function
{
public:
  StartFunction(const char *name, Instruction **insns,
                int first, int last);

  bool pass1();
};

#endif /* !__FUNCTION_HH__ */
