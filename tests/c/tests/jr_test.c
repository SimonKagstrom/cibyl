/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      jr_test.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   C frontend for the jr test
 *
 * $Id:$
 *
 ********************************************************************/
#include <test.h>

extern int jr_test_asm(void);

extern long long return_in_v1_asm(void);

/* The run-the-tests function */
void jr_run(void)
{
  int v;
  long long v2;

  v = jr_test_asm();
  if ( v != 0 )
    FAIL("jr_test: %d != 0\n", v);
  else
    PASS("jr_test: %d\n", v);

  v2 = return_in_v1_asm();
  if (v2 & 0x100000000LL)
    PASS("return_in_v1: %x:%08x\n", (unsigned int)(v2 >> 32), (unsigned int)(v2 & 0xffffffff) );
  else
    PASS("return_in_v1: %x:%08x != 0x1:Random\n", (unsigned int)(v2 >> 32), (unsigned int)(v2 & 0xffffffff) );
}
