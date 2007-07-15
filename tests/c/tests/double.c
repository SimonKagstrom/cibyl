/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      int.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Float tests
 *
 * $Id: double.c 13072 2007-01-17 19:02:12Z ska $
 *
 ********************************************************************/
#include <test.h>
#include <stdlib.h>
#include <math.h>

/* Functions to test */
binary_arithmetic_test_double(double_add, +);
binary_arithmetic_test_double(double_sub, -);
binary_arithmetic_test_double(double_mul, *);
binary_arithmetic_test_double(double_div, /);
binary_arithmetic_test_double(double_ge, >=);
binary_arithmetic_test_double(double_lt, <);
binary_arithmetic_test_double(double_le, <=);
binary_arithmetic_test_double(double_gt, >);
binary_arithmetic_test_double(double_eq, ==);

unary_arithmetic_test_double(double_neg, -);

typecast_test(double_f2i, double, int);
typecast_test(double_f2l, double, long);
typecast_test(double_f2ul, double, unsigned long);

/* The actual tests (add more here, especially regressions) */
static double_test_t double_bin_tests[] =
{
  BIN_OP(double_add, +, 1.1, 0.9),
  BIN_OP(double_add, +, 1.1, 0),
  BIN_OP(double_add, +, 1.1, NAN),
  BIN_OP(double_add, +, NAN, NAN),
  BIN_OP(double_add, +, INFINITY, INFINITY),

  BIN_OP(double_sub, -, 3.1, 0.9),
  BIN_OP(double_sub, -, 3.1, 122.9),
  BIN_OP(double_sub, -, NAN, NAN),
  BIN_OP(double_sub, -, NAN, NAN),
  BIN_OP(double_sub, -, INFINITY, INFINITY),

  BIN_OP(double_div, /, 5, 0.0),
  BIN_OP(double_div, /, 5, 1.0001),
  BIN_OP(double_div, /, 1.0, 23525321.0),

  BIN_OP(double_mul, *, 353556, 23525321.0),
  BIN_OP(double_mul, *, 353556, 0.0),
  BIN_OP(double_mul, *, -1.0, 1.0),
  BIN_OP(double_mul, *, -0.0000001, 0.000001),


  BIN_OP(double_ge, >=, -0.0000001, 0.000001),
  BIN_OP(double_ge, >=, 0.0000001, 0.000001),
  BIN_OP(double_ge, >=, 0.0000001, -0.000001),

  BIN_OP(double_lt, <, -0.0000001, 0.000001),
  BIN_OP(double_lt, <, 0.0000001, 0.000001),
  BIN_OP(double_lt, <, 0.0000001, -0.000001),

  BIN_OP(double_le, <=, -0.0000001, 0.000001),
  BIN_OP(double_le, <=, 0.0000001, 0.000001),
  BIN_OP(double_le, <=, 0.0000001, -0.000001),

  BIN_OP(double_gt, >, -0.0000001, 0.000001),
  BIN_OP(double_gt, >, 0.0000001, 0.000001),
  BIN_OP(double_gt, >, 0.0000001, -0.000001),

  BIN_OP(double_eq, ==, -0.0000001, 0.000001),
  BIN_OP(double_eq, ==, 0.0000001, 0.000001),
  BIN_OP(double_eq, ==, 0.0000001, -0.000001),
  BIN_OP(double_eq, ==, 0.0, 0.0),
  BIN_OP(double_eq, ==, -0.0, -0.0),
};

static double_un_test_t double_un_tests[] =
{
  UN_OP(double_neg, -, 1.1),
  UN_OP(double_neg, -, -1.1),
  /* After a problem in vsnprintf */
  TC_OP(double_f2i, double, int, 1.1),
  TC_OP(double_f2i, double, int, -1),
  TC_OP(double_f2i, double, int, 353556 * 23525321.0),
  TC_OP(double_f2l, double, long, 1.1),
  TC_OP(double_f2l, double, long, 353556 * 23525321.0),
  TC_OP(double_f2ul, double, unsigned long, -1.2),
  TC_OP(double_f2ul, double, unsigned long, 353556 * 23525321.0),
};

/* The run-the-tests function */
void double_run(void)
{
  run_test_bin_vector(double, double_test_t, double_bin_tests, "%f");
  run_test_un_vector(double, double_un_test_t, double_un_tests, "%f");
}
