/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
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

/* The run-the-tests function */
void jr_run(void)
{
  int v;

  v = jr_test_asm();
  if ( v != 0 )
    FAIL("jr_test: %d != 0\n", v);
  else
    PASS("jr_test: %d\n", v);
}
