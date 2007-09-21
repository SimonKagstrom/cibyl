/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      and.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   and
 *
 * $Id:$
 *
 ********************************************************************/
#if defined(HOST)
void and_run(void)
{
}
#else
#include <test.h>


#define andi_test(_con)                            \
unsigned int andi##_con(unsigned int in)           \
{                                                  \
  unsigned int out;                                \
  asm volatile("andi %[out], %[in], %[con]\n"      \
               : [out]"=r"(out)                    \
               : [con]"i"(_con), [in]"r"(in));     \
  return out;                                      \
}

andi_test(0xffff);
andi_test(0x1fff);

#define run_andi_test(a, con) \
{\
  unsigned int res = andi##con(a); \
  if (res != ((unsigned int)(a) & con)) \
    FAIL("andi %x & %x: %x != %x\n", a, con, res, ((a) & con)); \
  else \
    PASS("andi %x & %x: %x == %x\n", a, con, res, ((a) & con)); \
}

/* The run-the-tests function */
void and_run(void)
{
  run_andi_test(0xffffff, 0xffff);
  run_andi_test(0xffffff, 0x1fff);
  run_andi_test(0xff, 0xffff);
  run_andi_test(0xff, 0x1fff);
  run_andi_test(0x0,  0x1fff);
}
#endif
