/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      math.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl math
 *
 * $Id: math.h 14111 2007-03-10 08:37:37Z ska $
 *
 ********************************************************************/
#ifndef __MATH_H__
#define __MATH_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl-softfloat.h>

/* These are normally conditionally defined, but we skip that */
# define M_E            2.7182818284590452354   /* e */
# define M_LOG2E        1.4426950408889634074   /* log_2 e */
# define M_LOG10E       0.43429448190325182765  /* log_10 e */
# define M_LN2          0.69314718055994530942  /* log_e 2 */
# define M_LN10         2.30258509299404568402  /* log_e 10 */
# define M_PI           3.14159265358979323846  /* pi */
# define M_PI_2         1.57079632679489661923  /* pi/2 */
# define M_PI_4         0.78539816339744830962  /* pi/4 */
# define M_1_PI         0.31830988618379067154  /* 1/pi */
# define M_2_PI         0.63661977236758134308  /* 2/pi */
# define M_2_SQRTPI     1.12837916709551257390  /* 2/sqrt(pi) */
# define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
# define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */

/* Requires GCC >= 3.3 */
#define NAN       (__builtin_nanf(""))
#define INFINITY  (__builtin_inff())

extern double sin(double x);
extern float sinf(float x);

extern double cos(double x);
extern float cosf(float x);

extern double atan2(double x, double y);
extern float atan2f(float x, float y);

extern double atan(double x);
extern float atanf(float x);

extern double asin(double x);
extern float asinf(float x);

extern double acos(double x);
extern float acosf(float x);

extern double log10(double x);
extern float log10f(float x);

extern double log(double x);
extern float logf(float x);

extern double pow(double x, double y);
extern float powf(float x, float y);


extern double sqrt(double x);
extern float sqrtf(float x);

extern double exp(double x);
extern float expf(float x);

extern double floor(double x);
extern float floorf(float x);


static inline double fabs(double a)
{
  if (a < 0.0)
    return -a;
  return a;
}

static inline float fabsf(float a)
{
  if (a < 0.0)
    return -a;
  return a;
}

static inline int isnan(double A)
{
  double_union_t a;

  a.f = A;

  return __isnan_helper(&a.i);
}

#define isinf(x) \
   (sizeof (x) == sizeof (float)          \
    ? __isinff (x)                        \
    : __isinf (x))

static inline int __isinf(double A)
{
  double_union_t a;

  a.f = A;

  return __isinf_helper(&a.i);
}

static inline int __isinff(double A)
{
  float_union_t a;

  a.f = A;

  return __isinff_helper(a.i);
}

#if defined(__cplusplus)
}
#endif
#endif /* !__MATH_H__ */
