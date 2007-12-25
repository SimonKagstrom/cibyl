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
#include <stdint.h>

#include <cibyl-mips-regdef.h>

unsigned long cibyl_debug_regs[NREGS];
static unsigned long stack_start;

static const char *regnr_to_name[NREGS] =
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
  for (i=0; i < NREGS; i++)
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

/* --- Cibyl/Qemu implementation --- */
volatile uint32_t *cibyl_device = (volatile uint32_t*)0x10000000;

/* Exception handler for reserved instructions */
uint32_t __qemu_reserved(uint32_t *saved_regs,
			 uint32_t c0_status, uint32_t c0_cause, uint32_t c0_epc)
{
  uint32_t *insn = (uint32_t*)c0_epc;

  /* FIXME: Check if this really is valid */
  if ( (((*insn) >> 24) & 0xfe) == 0xfe )
    *cibyl_device = saved_regs[(*insn) & 0xffff];

  if ( (((*insn) >> 24) & 0xff) == 0xff )
    {
      *(cibyl_device+1) = *insn;
      /* This was a call, now read the return value */
      saved_regs[R_V0] = *cibyl_device;
    }

  return c0_epc + 4;
}

uint32_t __qemu_default(uint32_t *saved_regs,
			uint32_t c0_status, uint32_t c0_cause, uint32_t c0_epc)
{
  return c0_epc + 4;
}

