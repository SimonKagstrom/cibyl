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

  /**
   * Get the name of this function as it will appear in the Java
   * bytecode. Unique in the entire program.
   */
  const char *getName()
  {
    return this->name;
  }

  /**
   * Get the name of this function as it appears in the C source
   * code. Might not be unique (static functions with the same name).
   */
  const char *getRealName()
  {
    return this->realName;
  }

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

  virtual size_t getMaxStackHeight(void)
  {
    return this->maxStackHeight;
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
  size_t maxStackHeight;

  BasicBlock **bbs;
  char *name;
  char *realName;
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
