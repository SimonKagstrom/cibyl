/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      vmips.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Run tests from the vmips project
 *
 * $Id:$
 *
 ********************************************************************/
#if defined(HOST)
void vmips_run(void)
{
}
#else
#include <test.h>

extern int vmips_shifts(int a0, int a1, int a2, int a3); /* fool gcc */
extern int vmips_bltz(int *r, int *r2, int *r3, int a3);
extern int vmips_load(int *r, int *r2, int *r3, int a3);
extern int vmips_multu(int *r, int *r2, int *r3, int *r4);
extern int vmips_xor(int *r, int *r2, int *r3, int *r4);

/* The run-the-tests function */
void vmips_run(void)
{
  int r, r2, r3, r4;

  r = vmips_shifts(0, 0, 0, 0);
  if (r != 0)
    FAIL("vmips shifts: %x != %x\n", r, 0);
  else
    PASS("vmips shifts: %x == %x\n", r, 0);

  vmips_bltz(&r, &r2, &r3, 0);
  if (r != 1 || r2 != 1 || r3 != 0)
    FAIL("vmips bltz: %x,%x,%x != 1,1,0\n", r, r2, r3, 0);
  else
    PASS("vmips bltz: %x,%x,%x == 1,1,0\n", r, r2, r3, 0);

  vmips_load(&r, &r2, &r3, 0);
  if (r != 0xDEADBEEF || r2 != 0x00001234 || r3 != 0x00000012)
    FAIL("vmips load: %x,%x,%x != 0xdeadbeef,0x1234,0x12\n", r, r2, r3, 0);
  else
    PASS("vmips load: %x,%x,%x == 0xdeadbeef,0x1234,0x12\n", r, r2, r3, 0);

  vmips_multu(&r, &r2, &r3, &r4);
  if (r != 0x7ffffffe || r2 != 0x80000001 || r3 != 0x7fffffff  || r4 != 0xfffffffe)
    FAIL("vmips multu: %x,%x,%x,%x != 0x7ffffffe,0x80000001,0x7fffffff,0xffffffff", r, r2,r3,r4);
  else
    PASS("vmips multu: %x,%x,%x,%x == 0x7ffffffe,0x80000001,0x7fffffff,0xffffffff", r, r2,r3,r4);

  r = vmips_xor(0,0,0,0);
  if (r != 0)
    PASS("vmips xor: %d != %d", r);
  else
    PASS("vmips xor: %d == %d", r);
}
#endif
