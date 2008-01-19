/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      64-bit-return.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Test of 64-bit return values
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdint.h>
#include <test.h>

uint64_t ll_ret_test(uint64_t v)
{
  return v;
}

/* The run-the-tests function */
void ll_ret_run(void)
{
  uint64_t res;
  uint64_t v;

  v = 0xffffffff00000000ll;
  res = ll_ret_test(v);
  if (res != v)
    FAIL("64 bit return %llx != %llx\n", res, v);
  else
    PASS("64 bit return %llx == %llx\n", res, v);

  v = 0x00000000ffffffffll;
  res = ll_ret_test(v);
  if (res != v)
    FAIL("64 bit return %llx != %llx\n", res, v);
  else
    PASS("64 bit return %llx == %llx\n", res, v);

  v = 0xf00000000fffffffll;
  res = ll_ret_test(v);
  if (res != v)
    FAIL("64 bit return %llx != %llx\n", res, v);
  else
    PASS("64 bit return %llx == %llx\n", res, v);
}
