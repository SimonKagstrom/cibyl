/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      registerallocator.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   The register allocator class
 *
 * $Id:$
 *
 ********************************************************************/
#include <string.h>
#include <registerallocator.hh>

RegisterAllocator::RegisterAllocator()
{
  memset(this->mips_to_local, 0, sizeof(this->mips_to_local));
  memset(this->mips_to_static, 0, sizeof(this->mips_to_static));

  this->n_locals = 0;
}

static int allocateRegister(int n, MIPS_register_t reg, MIPS_register_t *sorted, int *usage)
{
  sorted[n++] = reg;
  usage[reg] = 0;
  return n;
}

void RegisterAllocator::setAllocation(int *registerUsage)
{
  MIPS_register_t sorted[N_REGS];
  int usage[N_REGS];
  int n = 0;

  memset(this->mips_to_local, -1, sizeof(this->mips_to_local));
  memset(sorted, 0, sizeof(sorted));
  memcpy(usage, registerUsage, sizeof(usage));

  /* Allocate initial registers */
  if (registerUsage[R_SP] > 0)
    n = allocateRegister(n, R_SP, sorted, usage);
  if (registerUsage[R_FNA] > 0)
    n = allocateRegister(n, R_FNA, sorted, usage);
  if (registerUsage[R_A0] > 0)
    n = allocateRegister(n, R_A0, sorted, usage);
  if (registerUsage[R_A1] > 0)
    n = allocateRegister(n, R_A1, sorted, usage);
  if (registerUsage[R_A2] > 0)
    n = allocateRegister(n, R_A2, sorted, usage);
  if (registerUsage[R_A3] > 0)
    n = allocateRegister(n, R_A3, sorted, usage);

  /* Allocate the rest of the registers */
  int largest;
  do
    {
      largest = 0;
      MIPS_register_t largestIndex = R_ZERO;
      for (int i = 0; i < N_REGS; i++)
	{
	  if (usage[i] > 0 && usage[i] > largest)
	    {
	      largest = usage[i];
	      largestIndex = (MIPS_register_t)i;
	    }
	}
      if (largest != 0)
	n = allocateRegister(n, largestIndex, sorted, usage);
    } while (largest != 0);

  /* Setup the register to local mapping */
  for (int i = 0; i < n; i++)
    this->mips_to_local[sorted[i]] = i;

  this->n_locals = n;
}

bool RegisterAllocator::regIsStatic(MIPS_register_t reg)
{
  return this->regToStatic(reg) != NULL;
}

const char *RegisterAllocator::regToStatic(MIPS_register_t reg)
{
  return this->mips_to_static[reg];
}

int RegisterAllocator::regToLocal(MIPS_register_t reg)
{
  return this->mips_to_local[reg];
}

RegisterAllocator *regalloc;
