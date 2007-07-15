/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      libm.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   libm-stuff
 *
 * $Id: libm.c 14112 2007-03-10 08:37:50Z ska $
 *
 ********************************************************************/
#include <test.h>
#include <math.h>

unary_arithmetic_test_float(float_sin, sinf);
unary_arithmetic_test_float(float_cos, cosf);
unary_arithmetic_test_float(float_atan, atanf);
unary_arithmetic_test_float(float_asin, asinf);
unary_arithmetic_test_float(float_acos, acosf);
unary_arithmetic_test_float(float_sqrt, sqrtf);
unary_arithmetic_test_float(float_floor, floorf);

unary_arithmetic_test_double(double_sin, sin);
unary_arithmetic_test_double(double_cos, cos);
unary_arithmetic_test_double(double_atan, atan);
unary_arithmetic_test_double(double_asin, asin);
unary_arithmetic_test_double(double_acos, acos);
unary_arithmetic_test_double(double_sqrt, sqrt);
unary_arithmetic_test_double(double_floor, floor);

binary_arithmetic_test_fn_float(float_atan2f, atan2f);

binary_arithmetic_test_fn_double(float_atan2, atan2);

#if 0
unary_arithmetic_test_float(float_log, logf);
unary_arithmetic_test_double(double_log, log);

unary_arithmetic_test_float(float_log10, log10f);
unary_arithmetic_test_float(float_exp, expf);
binary_arithmetic_test_fn_float(float_pow, powf);

binary_arithmetic_test_fn_double(double_pow, pow);
unary_arithmetic_test_double(double_exp, exp);
unary_arithmetic_test_double(double_log10, log10);
#endif

static float_un_test_t float_un_tests[] =
{
  /* From python */
  FN_OP(float_sin, sinf, 1.1, 0.89120736006143542),
  FN_OP(float_sin, sinf, -M_PI, -1.2246063538223773e-16),
  FN_OP(float_sin, sinf, 3*(M_PI/2), -1.0),

  FN_OP(float_cos, cosf, 99.0, 0.039820880393138899),
  FN_OP(float_cos, cosf, 0, 1.0),

  FN_OP(float_atan, atanf, 0.0, 0.0),
  FN_OP(float_atan, atanf, M_PI, 1.2626272556789118),

  FN_OP(float_asin, asinf, 0.0, 0.0),
  FN_OP(float_asin, asinf, 0.5, 0.52359877559829893),

  FN_OP(float_acos, acosf, 0.0, 1.5707963267948966),
  FN_OP(float_acos, acosf, 0.5, 1.0471975511965979),

  FN_OP(float_sqrt, sqrt, 3, 1.7320508075688772),
  FN_OP(float_sqrt, sqrt, 0, 0.0),
  FN_OP(float_sqrt, sqrt, 4, 2.0),

#if 0
  FN_OP(float_log, logf, 0.9, -0.10536051565782628),
  FN_OP(float_exp, expf, 4, 54.598150033144236),
  FN_OP(float_log10, log10f, 2, 0.3010299956639812),
  FN_OP(float_log10, log10f, 0.9, -0.045757490560675115),
#endif
};

static float_test_t float_bin_tests[] =
{
  FN_BIN_OP(float_atan2f, atan2f, M_PI, 0, 1.5707963267948966),
  FN_BIN_OP(float_atan2f, atan2f, M_PI, M_PI, 0.78539816339744828),
  FN_BIN_OP(float_atan2f, atan2f, 0, 0, 0), /* Fails */
#if 0
  FN_BIN_OP(float_pow, powf, 2, 3, 8.0),
  FN_BIN_OP(float_pow, powf, 2, 0.9, 1.8660659830736148),
  FN_BIN_OP(float_pow, powf, 0, 2, 0),
#endif
};

static double_un_test_t double_un_tests[] =
{
  /* From python */
  FN_OP(double_sin, sin, 1.1, 0.89120736006143542),
  FN_OP(double_sin, sin, -M_PI, -1.2246063538223773e-16),
  FN_OP(double_sin, sin, 3*(M_PI/2), -1.0),

  FN_OP(double_cos, cos, 99.0, 0.039820880393138899),
  FN_OP(double_cos, cos, 0, 1.0),

  FN_OP(double_atan, atan, 0.0, 0.0),
  FN_OP(double_atan, atan, M_PI, 1.2626272556789118),

  FN_OP(double_asin, asin, 0.0, 0.0),
  FN_OP(double_asin, asin, 0.5, 0.52359877559829893),

  FN_OP(double_acos, acos, 0.0, 1.5707963267948966),
  FN_OP(double_acos, acos, 0.5, 1.0471975511965979),

  FN_OP(double_sqrt, sqrt, 3, 1.7320508075688772),
  FN_OP(double_sqrt, sqrt, 0, 0.0),
  FN_OP(double_sqrt, sqrt, 4, 2.0),

#if 0
  FN_OP(double_log, log, 0.9, -0.10536051565782628),
  FN_OP(double_exp, exp, 4, 54.598150033144236),
  FN_OP(double_log10, log10, 2, 0.3010299956639812),
  FN_OP(double_log10, log10, 0.9, -0.045757490560675115),
#endif
};

static double_test_t double_bin_tests[] =
{
  FN_BIN_OP(float_atan2, atan2, M_PI, 0, 1.5707963267948966),
  FN_BIN_OP(float_atan2, atan2, M_PI, M_PI, 0.78539816339744828),
  FN_BIN_OP(float_atan2, atan2, 0, 0, 0), /* Fails */
#if 0
  FN_BIN_OP(double_pow, pow, 2, 3, 8.0),
  FN_BIN_OP(double_pow, pow, 2, 0.9, 1.8660659830736148),
  FN_BIN_OP(double_pow, pow, 0, 2, 0),
#endif
};


/* The run-the-tests function */
void libm_run(void)
{
  run_test_bin_vector(float, float_test_t, float_bin_tests, "%f");
  run_test_un_vector(float, float_un_test_t, float_un_tests, "%f");
  run_test_bin_vector(double, double_test_t, double_bin_tests, "%f");
  run_test_un_vector(double, double_un_test_t, double_un_tests, "%f");
}
