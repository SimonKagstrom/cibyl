/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      registerallocator.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Register allocator
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __REGISTERALLOCATOR_HH__
#define __REGISTERALLOCATOR_HH__

#include <mips.hh>

class Instruction;

class RegisterAllocator
{
public:
  RegisterAllocator();

  void setAllocation(int *registerUsage);

  bool regIsStatic(MIPS_register_t reg);

  const char *regToStatic(MIPS_register_t reg);

  bool regIsAllocated(MIPS_register_t reg)
  {
    return (mips_to_local[reg] != -1);
  }

  int regToLocal(MIPS_register_t reg);

  int getNumberOfLocals()
  {
    return this->n_locals;
  }

private:
  int mips_to_local[N_REGS];
  const char *mips_to_static[N_REGS];

  int n_locals;
};

extern RegisterAllocator *regalloc;

#endif /* !__REGISTERALLOCATOR_HH__ */
