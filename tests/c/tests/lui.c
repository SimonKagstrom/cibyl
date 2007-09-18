/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      lui.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   set less than tests
 *
 * $Id:$
 *
 ********************************************************************/
#if defined(HOST)
void lui_run(void)
{
}
#else
#include <test.h>

#define lui_test(_upper)                           \
unsigned int lui##_upper(void)                     \
{                                                  \
  unsigned int upper = _upper;                     \
  unsigned int out;                                \
  asm volatile("lui %[out], %[upper]\n"            \
               : [out]"=r"(out)                    \
               : [upper]"i"(upper));               \
  return out;                                      \
}

#define lui_ori_test(_upper, _lower)               \
unsigned int luiori##_upper##_lower(void)             \
{                                                  \
  unsigned int out;                                \
  asm volatile("lui %[out], %[upper]\n"            \
               "ori %[out], %[out], %[lower]\n"            \
               : [out]"=r"(out)                    \
               : [upper]"i"(_upper), [lower]"i"(_lower));               \
  return out;                                      \
}

#define lui_addiu_test(_upper, _lower)               \
unsigned int luiaddiu##_upper(void)             \
{                                                  \
  unsigned int out;                                \
  asm volatile("lui %[out], %[upper]\n"            \
               "addiu %[out], %[out], %[lower]\n"          \
               : [out]"=r"(out)                    \
               : [upper]"i"(_upper), [lower]"i"(_lower));               \
  return out;                                      \
}

lui_test(0xaaaa);
lui_ori_test(0x2, 0xadab);
lui_ori_test(0x5555, 0x5555);
lui_ori_test(0xaaaa, 0xaaaa);
lui_ori_test(0xfff, 0xffff);
lui_ori_test(0x3, 0xf85e);
lui_addiu_test(0x8, -17992);
lui_addiu_test(0xaaaa, 0xaaaa);

#define run_lui_test(upper)              \
  {                                      \
    unsigned int out = lui##upper();     \
    if (out != (upper<<16))              \
       FAIL("lui %x: %x != %x", upper, out, upper << 16); \
    else                                 \
       PASS("lui %x: %x == %x", upper, out, upper << 16); \
  }

#define run_lui_ori_test(upper,lower)    \
  {                                      \
    unsigned int out = luiori##upper##lower();     \
    if (out != ((upper<<16) | lower))    \
       FAIL("lui %x | %x: %x != %x", upper, lower, out, (upper << 16) | lower); \
    else                                 \
       PASS("lui %x | %x: %x == %x", upper, lower, out, (upper << 16) | lower); \
  }

#define run_lui_addiu_test(upper,lower)    \
  {                                      \
    unsigned int out = luiaddiu##upper();     \
    if (out != ((upper<<16) + lower))    \
       FAIL("lui %x + %x: %x != %x", upper, lower, out, (upper << 16) + lower); \
    else                                 \
       PASS("lui %x + %x: %x == %x", upper, lower, out, (upper << 16) + lower); \
  }

/* The run-the-tests function */
void lui_run(void)
{
  run_lui_test(0xaaaa);
  run_lui_ori_test(0x2, 0xadab);
  run_lui_ori_test(0x5555, 0x5555);
  run_lui_ori_test(0xaaaa, 0xaaaa);
  run_lui_ori_test(0xfff, 0xffff);
  run_lui_addiu_test(0xaaaa, 0xaaaa);
  run_lui_addiu_test(0x8, -17992);
}
#endif
