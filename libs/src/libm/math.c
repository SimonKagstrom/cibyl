/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      math.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   libm implementation for Cibyl
 *
 * $Id: math.c 14109 2007-03-10 08:19:13Z ska $
 *
 ********************************************************************/
#include <cibyl-softfloat.h>

/* Macros to automate some boring work */
#define FLOAT_CALL1(name, p1)      \
extern float name(float p1)        \
{				   \
  float_union_t a;		   \
  float_union_t res;		   \
				   \
  a.f = p1;			   \
				   \
  res.i = __##name##_helper(a.i);  \
				   \
  return res.f;			   \
}

#define FLOAT_CALL2(name, p1, p2)  \
extern float name(float p1, float p2) \
{                                  \
  float_union_t a, b;              \
  float_union_t res;               \
				   \
  a.f = p1;			   \
  b.f = p2;			   \
				   \
  res.i = __##name##_helper(a.i, b.i); \
				   \
  return res.f;			   \
}


#define DOUBLE_CALL1(name, p1)     \
extern double name(double p1)      \
{                                  \
  double_union_t a;                \
  double_union_t res;              \
				   \
  a.f = p1;			   \
				   \
  __##name##_helper(&res.i, &a.i); \
				   \
  return res.f;			   \
}

#define DOUBLE_CALL2(name, p1, p2) \
extern double name(double p1, double p2) \
{                                  \
  double_union_t a, b;             \
  double_union_t res;              \
				   \
  a.f = p1;			   \
  b.f = p2;			   \
				   \
  __##name##_helper(&res.i, &a.i, &b.i);   \
				   \
  return res.f;			   \
}

/* Support functions for the libm stuff. These call into Java
 * functionality
 */
DOUBLE_CALL1(sin, x);
DOUBLE_CALL1(cos, x);
DOUBLE_CALL1(atan, x);
DOUBLE_CALL1(asin, x);
DOUBLE_CALL1(acos, x);
DOUBLE_CALL1(log10, x);
DOUBLE_CALL1(log, x);
DOUBLE_CALL1(exp, x);
DOUBLE_CALL1(floor, x);
DOUBLE_CALL1(sqrt, x);

DOUBLE_CALL2(atan2, A, B);
DOUBLE_CALL2(pow, A, B);


FLOAT_CALL1(sinf, x);
FLOAT_CALL1(cosf, x);
FLOAT_CALL1(atanf, x);
FLOAT_CALL1(asinf, x);
FLOAT_CALL1(acosf, x);
FLOAT_CALL1(log10f, x);
FLOAT_CALL1(logf, x);
FLOAT_CALL1(expf, x);
FLOAT_CALL1(floorf, x);
FLOAT_CALL1(sqrtf, x);

FLOAT_CALL2(atan2f, A, B);
FLOAT_CALL2(powf, A, B);
