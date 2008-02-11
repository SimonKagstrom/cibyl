/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      basicblock.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Basic block defs
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __BASICBLOCK_HH__
#define __BASICBLOCK_HH__

#include <instruction.hh>
#include <entity.hh>

typedef enum
{
  NORMAL,
  PROLOGUE,
  EPILOGUE,
} bb_type_t;

class Function;

class BasicBlock : public CodeBlock
{
public:
  BasicBlock(Instruction **insns, bb_type_t type,
	     int first, int last);

  void setType(bb_type_t type)
  {
    this->type = type;
  }

  bool pass1();

  bool pass2();

  int fillDestinations(int *p);

  int fillSources(int *p);

private:
  void commentInstruction(Instruction *insn);

  void traceRegisterValues(Instruction *insn);

  int n_insns;
  bb_type_t type;
  Instruction **instructions;
};

#endif /* !__BASICBLOCK_HH__ */
