/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      shifts.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   set less than tests
 *
 * $Id:$
 *
 ********************************************************************/
#if defined(HOST)
void shifts_run(void)
{
}
#else
#include <test.h>

void srav(unsigned int a, unsigned int b, unsigned int correct)
{
  unsigned int out;
  asm volatile("srav %[out], %[a], %[b]\n"
               : [out]"=r"(out) : [a]"r"(a), [b]"r"(b));
  if (out != correct)
    FAIL("srav %x,%x: %x != %x\n", a, b, out, correct);
  else
    PASS("srav %x,%x: %x == %x\n", a, b, out, correct);
}

void sllv(unsigned int a, unsigned int b, unsigned int correct)
{
  unsigned int out;
  asm volatile("sllv %[out], %[a], %[b]\n"
               : [out]"=r"(out) : [a]"r"(a), [b]"r"(b));
  if (out != correct)
    FAIL("sllv %x,%x: %x != %x\n", a, b, out, correct);
  else
    PASS("sllv %x,%x: %x == %x\n", a, b, out, correct);
}

#define srl_test(_con)                             \
unsigned int srl##_con(unsigned int in)            \
{                                                  \
  unsigned int out;                                \
  asm volatile("srl %[out], %[in], %[con]\n"       \
               : [out]"=r"(out)                    \
               : [con]"i"(_con), [in]"r"(in));     \
  return out;                                      \
}

srl_test(0x18);
srl_test(0x0);
srl_test(0x1);

#define run_srav_test(a, b) \
     srav(a, b, a >> b);

#define run_sllv_test(a, b) \
     sllv(a, b, a << b);

#define run_srl_test(a, con) \
{\
  unsigned int res = srl##con(a); \
  if (res != ((unsigned int)(a) >> con)) \
    FAIL("srl %x >> %x: %x != %x\n", a, con, res, ((a) >> con)); \
  else \
    PASS("srl %x >> %x: %x != %x\n", a, con, res, ((a) >> con)); \
}

/* The run-the-tests function */
void shifts_run(void)
{
  run_srav_test( (1<<31), 31);
  run_srav_test( -1, 16);
  run_srav_test( 0, 16);
  run_srav_test( 4, 5);

  run_sllv_test( 1, 31);
  run_sllv_test( (1<<31), 1);
  run_sllv_test( 0, 31);

  run_srl_test(0xa, 0x0);
  run_srl_test(0x1, 0x0);
  run_srl_test(-1, 0x1);

  run_srl_test(0xa, 0x18);
  run_srl_test(1 << 0x18, 0x18);
  run_srl_test(1 << 0x17, 0x18);
  run_srl_test(1 << 0x19, 0x18);
}
#endif
