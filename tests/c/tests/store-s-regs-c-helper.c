/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      store-s-regs-c-helper.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   C frontend for store-s-regs.c
 *
 * $Id:$
 *
 ********************************************************************/
#include <test.h>

extern void store_s_regs(void);

void store_s_regs_c_helper(int a0, int a1, int a2, int a3, int via_stack)
{
  if (via_stack != 16444)
    FAIL("Passing via stack: %d", via_stack);
  else
    PASS("Passing via stack: %d", via_stack);
}

/* The run-the-tests function */
void store_s_regs_run(void)
{
  store_s_regs();
}
