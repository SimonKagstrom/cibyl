/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      run-tests.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:
 *
 * $Id: run-tests.c 13859 2007-02-25 15:50:13Z ska $
 *
 ********************************************************************/
#include "test.h"

int verbose = VERBOSE;
int n_fail;
int n_pass;


typedef struct
{
  void (*fn)(void);
  const char *name;
} test_run_t;

#define TEST(fn) { fn, #fn}


extern void int_run(void);
extern void unsigned_int_run(void);
extern void char_run(void);
extern void varargs_run(void);
extern void float_run(void);
extern void double_run(void);
extern void libm_run(void);
extern void memory_run(void);
extern void exceptions_run(void);
extern void file_operations_run(void);
extern void relocation_run(void);
extern void jr_run(void);
extern void j2me_run(void);
extern void slt_run(void);
extern void lui_run(void);
extern void shifts_run(void);
extern void and_run(void);
extern void vmips_run(void);
extern void ll_ret_run(void);
extern void malloc_run(void);
extern void function_run(void);

test_run_t all_tests[] =
{
  TEST(int_run),
  TEST(unsigned_int_run),
  TEST(char_run),
  TEST(float_run),
  TEST(double_run),
  TEST(libm_run),
  TEST(varargs_run),
  TEST(memory_run),
  TEST(jr_run),
  TEST(j2me_run),
  TEST(slt_run),
  TEST(lui_run),
  TEST(and_run),
  TEST(shifts_run),
  TEST(vmips_run),
  TEST(ll_ret_run),
  TEST(function_run),
};


void test_run_all_tests(int run_fileops)
{
  test_run_t *test;

  test_output("Running tests...");

  n_fail = 0;
  n_pass = 0;

  for (test = all_tests;
       test != &all_tests[sizeof(all_tests) / sizeof(test_run_t)];
       test++)
    {
      test->fn();
    }
  if (run_fileops)
    {
      /* Delay these to avoid having to allow file access too often */
      malloc_run();
      exceptions_run();
      file_operations_run();
      relocation_run();
    }

  test_output("Ran %d tests, %d failures, %d succeded\n", n_fail + n_pass, n_fail, n_pass);
}
