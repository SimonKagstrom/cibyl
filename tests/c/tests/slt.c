/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      slt.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   set less than tests
 *
 * $Id:$
 *
 ********************************************************************/
#if defined(HOST)
void slt_run(void)
{
}
#else
#include <test.h>

int slti_0xffff(int v)
{
  int out;
  asm volatile("slti %[out], %[v], 0xffff\n"
               : [out]"=r"(out) : [v]"r"(v));
  return out;
}

unsigned int sltiu_0xffff(unsigned int v)
{
  unsigned int out;
  asm volatile("sltiu %[out], %[v], 0xffff\n"
               : [out]"=r"(out) : [v]"r"(v));
  return out;
}

int slti_0x0(int v)
{
  int out;
  asm volatile("slti %[out], %[v], 0x0\n"
               : [out]"=r"(out) : [v]"r"(v));
  return out;
}

unsigned int sltiu_0x0(unsigned int v)
{
  unsigned int out;
  asm volatile("sltiu %[out], %[v], 0x0\n"
               : [out]"=r"(out) : [v]"r"(v));
  return out;
}


void sltu(unsigned int a, unsigned int b, int correct)
{
  unsigned int out;
  asm volatile("sltu %[out], %[a], %[b]\n"
               : [out]"=r"(out) : [a]"r"(a), [b]"r"(b));
  if (out != correct)
    FAIL("slt %d,%d: %d != %d\n", a, b, out, correct);
  else
    PASS("slt %d,%d: %d == %d\n", a, b, out, correct);
}

void slt(int a, int b, int correct)
{
  unsigned int out;
  asm volatile("slt %[out], %[a], %[b]\n"
               : [out]"=r"(out) : [a]"r"(a), [b]"r"(b));
  if (out != correct)
    FAIL("slt %d,%d: %d != %d\n", a, b, out, correct);
  else
    PASS("slt %d,%d: %d == %d\n", a, b, out, correct);
}


void test_0xffff(int v, int exp, int expu)
{
  int res = slti_0xffff(0x0);
  int res2 = sltiu_0xffff(0x0);

  if (res != exp)
    FAIL("slti %x < 0xffff, %x != %x", v, res, exp);
  else
    PASS("slti %x < 0xffff, %x == %x", v, res, exp);

  if (res2 != expu)
    FAIL("sltiu %x < 0xffff, %x != %x", v, res2, expu);
  else
    PASS("sltiu %x < 0xffff, %x == %x", v, res2, expu);
}

void test_0x0(int v, int exp, int expu)
{
  int res = slti_0x0(0x0);
  int res2 = sltiu_0x0(0x0);

  if (res != exp)
    FAIL("slti %x < 0xffff, %x != %x", v, res, exp);
  else
    PASS("slti %x < 0xffff, %x == %x", v, res, exp);

  if (res2 != expu)
    FAIL("sltiu %x < 0xffff, %x != %x", v, res2, expu);
  else
    PASS("sltiu %x < 0xffff, %x == %x", v, res2, expu);
}

#define run_slt_test(a, b) \
     sltu(a, b, (unsigned int)a < (unsigned int)b); \
     slt(a, b, a < b);

/* The run-the-tests function */
void slt_run(void)
{
  test_0xffff(0, 1 < -1, 1 < 0xffff);
  test_0xffff(0x10000, 0x10000 < -1, 1 < 0xffff);

  test_0x0(0, 1 < 0, 1 < 0);
  test_0x0(0x10000, 0x10000 < -1, 1 < 0);

  run_slt_test(0x0, 0x0);
  run_slt_test(0x0, 0x1);
  run_slt_test(0x1, 0x0);
  run_slt_test(-1, -1);
  run_slt_test(-1, 0);
  run_slt_test(0, -1);
}
#endif
