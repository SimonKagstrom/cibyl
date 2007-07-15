/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      int.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Float tests
 *
 * $Id: float.c 13068 2007-01-17 18:49:35Z ska $
 *
 ********************************************************************/
#include <test.h>
#include <stdlib.h>
#include <math.h>

/* Functions to test */
binary_arithmetic_test_float(float_add, +);
binary_arithmetic_test_float(float_sub, -);
binary_arithmetic_test_float(float_mul, *);
binary_arithmetic_test_float(float_div, /);
binary_arithmetic_test_float(float_ge, >=);
binary_arithmetic_test_float(float_lt, <);
binary_arithmetic_test_float(float_le, <=);
binary_arithmetic_test_float(float_gt, >);
binary_arithmetic_test_float(float_eq, ==);

unary_arithmetic_test_float(float_neg, -);

typecast_test(float_f2i, float, int);
typecast_test(float_f2l, float, long);
typecast_test(float_f2ul, float, unsigned long);

/* The actual tests (add more here, especially regressions) */
static float_test_t float_tests[] =
{
  BIN_OP(float_add, +, 1.1, 0.9),
  BIN_OP(float_add, +, 1.1, 0),
  BIN_OP(float_add, +, 1.1, NAN),
  BIN_OP(float_add, +, NAN, NAN),
  BIN_OP(float_add, +, INFINITY, INFINITY),

  BIN_OP(float_sub, -, 3.1f, 0.9f), /* The cast from doubles will give a inexact result */
  BIN_OP(float_sub, -, 3.1, 122.9),
  BIN_OP(float_sub, -, NAN, NAN),
  BIN_OP(float_sub, -, NAN, NAN),
  BIN_OP(float_sub, -, INFINITY, INFINITY),

  BIN_OP(float_div, /, 5, 0.0),
  BIN_OP(float_div, /, 5, 1.0001f), /* Ditto */
  BIN_OP(float_div, /, 1.0, 23525321.0),

  BIN_OP(float_mul, *, 353556, 23525321.0),
  BIN_OP(float_mul, *, 353556, 0.0),
  BIN_OP(float_mul, *, -1.0, 1.0),
  BIN_OP(float_mul, *, -0.0000001, 0.000001),

  BIN_OP(float_ge, >=, -0.0000001, 0.000001),
  BIN_OP(float_ge, >=, 0.0000001, 0.000001),
  BIN_OP(float_ge, >=, 0.0000001, -0.000001),

  BIN_OP(float_lt, <, -0.0000001, 0.000001),
  BIN_OP(float_lt, <, 0.0000001, 0.000001),
  BIN_OP(float_lt, <, 0.0000001, -0.000001),

  BIN_OP(float_le, <=, -0.0000001, 0.000001),
  BIN_OP(float_le, <=, 0.0000001, 0.000001),
  BIN_OP(float_le, <=, 0.0000001, -0.000001),

  BIN_OP(float_gt, >, -0.0000001, 0.000001),
  BIN_OP(float_gt, >, 0.0000001, 0.000001),
  BIN_OP(float_gt, >, 0.0000001, -0.000001),

  BIN_OP(float_eq, ==, -0.0000001, 0.000001),
  BIN_OP(float_eq, ==, 0.0000001, 0.000001),
  BIN_OP(float_eq, ==, 0.0000001, -0.000001),
};

static float_un_test_t float_un_tests[] =
{
  UN_OP(float_neg, -, 1.1),
  UN_OP(float_neg, -, -1.1),

  TC_OP(float_f2i, float, int, 1.1),
  TC_OP(float_f2i, float, int, -1),
  TC_OP(float_f2i, float, int, 353556 * 23525321.0),
  TC_OP(float_f2l, float, long, 1.1),
  TC_OP(float_f2l, float, long, 353556 * 23525321.0),
  TC_OP(float_f2ul, float, unsigned long, -1.2),
  TC_OP(float_f2ul, float, unsigned long, 353556 * 23525321.0),
};


/* The run-the-tests function */
void float_run(void)
{
  run_test_bin_vector(float, float_test_t, float_tests, "%f");
  run_test_un_vector(float, float_un_test_t, float_un_tests, "%f");
}
