/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      debug.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl debugging stuff
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdio.h>

#include "regnames.h"

unsigned long cibyl_debug_regs[N_REGS];
static unsigned long stack_start;

static const char *regnr_to_name[N_REGS] =
{
  [R_ZERO] = "zero",   [R_AT] = "  at", [R_V0] = "  v0",
  [R_V1]   = "  v1",   [R_A0] = "  a0", [R_A1] = "  a1",
  [R_A2]   = "  a2",   [R_A3] = "  a3", [R_T0] = "  t0",
  [R_T1]   = "  t1",   [R_T2] = "  t2", [R_T3] = "  t3",
  [R_T4]   = "  t4",   [R_T5] = "  t5", [R_T6] = "  t6",
  [R_T7]   = "  t7",   [R_S0] = "  s0", [R_S1] = "  s1",
  [R_S2]   = "  s2",   [R_S3] = "  s3", [R_S4] = "  s4",
  [R_S5]   = "  s5",   [R_S6] = "  s6", [R_S7] = "  s7",
  [R_T8]   = "  t8",   [R_T9] = "  t9", [R_K0] = "  k0",
  [R_K1]   = "  k1",   [R_GP] = "  gp", [R_SP] = "  sp",
  [R_FP]   = "  fp",   [R_RA] = "  ra",
};

static void __attribute__((constructor)) setup_debug_support(void)
{
  register unsigned long sp asm("$29");

  stack_start = sp;
}


static int is_valid(unsigned int addr)
{
  if (addr <= stack_start)
    return 1;
  return 0;
}


void cibyl_dump_stack_helper(void)
{
  int i;
  unsigned long *regs = cibyl_debug_regs;
  unsigned long *p_stk = (unsigned long*)regs[R_SP];

  printf("Registers:");
  for (i=0; i < N_REGS; i++)
    {
      if (i % 4 == 0)
	printf("\n");
      printf("%s: 0x%08x ", regnr_to_name[i], (unsigned int)regs[i]);
    }

  printf("\n\nStack (starts at 0x%lx):", stack_start);
  for (i=0; i < 16; i++)
    {
      /* Check that the stack pointer is within valid memory */
      if (!is_valid( (unsigned long)&p_stk[i]) )
	break;
      if (i % 5 == 0)
	  printf("\n"
		 "0x%08x: ", (unsigned int)&(p_stk[i]));
      printf("0x%08x  ", (unsigned int)p_stk[i]);
    }
  printf("\n");
}
