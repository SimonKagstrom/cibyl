/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      soft-float.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description: libgcc-compatible soft float implementation. Based on
 *              ghostscript soft-float code.
 *
 * $Id: soft-float.c 13526 2007-02-09 17:19:58Z ska $
 *
 ********************************************************************/
#include <cibyl-softfloat.h>
#include <stdlib.h> /* atof */
#include <ctype.h> /* atof */

#define FLOAT_ARITHETIC_1(name, p1) \
float name(float p1)                          \
{                                             \
  float_union_t a, res;                       \
                                              \
  a.f = p1;                                   \
                                              \
  res.i = name##_helper(a.i);                 \
  return res.f;                               \
}

#define FLOAT_ARITHETIC_2(name, p1, p2) \
float name(float p1, float p2)                \
{                                             \
  float_union_t a, b, res;                    \
                                              \
  a.f = p1; b.f = p2;                         \
                                              \
  res.i = name##_helper(a.i, b.i);            \
  return res.f;                               \
}

#define DOUBLE_ARITHMETIC_1(name, p1) \
double name(double p1)                         \
{                                              \
  double_union_t a, res;                       \
                                               \
  a.f = p1;                                    \
                                               \
  name##_helper(&res.i, &a.i);                 \
  return res.f;                                \
}

#define DOUBLE_ARITHMETIC_2(name, p1, p2) \
double name(double p1, double p2)              \
{                                              \
  double_union_t a, b, res;                    \
                                               \
  a.f = p1; b.f = p2;                          \
                                               \
  name##_helper(&res.i, &a.i, &b.i);           \
  return res.f;                                \
}

FLOAT_ARITHETIC_2(__addsf3, A, B);   /* float __addsf3 (float A, float B); */
DOUBLE_ARITHMETIC_2(__adddf3, A, B);  /* double __adddf3 (double A, double B); */

FLOAT_ARITHETIC_2(__subsf3, A, B);   /* float __subsf3 (float A, float B) */
DOUBLE_ARITHMETIC_2(__subdf3, A, B);  /* double __subdf3 (double A, double B) */

FLOAT_ARITHETIC_2(__mulsf3, A, B);   /* float __mulsf3 (float A, float B) */
DOUBLE_ARITHMETIC_2(__muldf3, A, B);  /* double __muldf3 (double A, double B) */

FLOAT_ARITHETIC_2(__divsf3, A, B);   /* float __divsf3 (float A, float B) */
DOUBLE_ARITHMETIC_2(__divdf3, A, B);  /* double __divdf3 (double A, double B) */

FLOAT_ARITHETIC_1(__negsf2, A);   /* float __negsf2 (float A) */
DOUBLE_ARITHMETIC_1(__negdf2, A);  /* double __negdf2 (double A) */

double __extendsfdf2 (float A)
{
  float_union_t a;
  double_union_t res;

  a.f = A;
  __extendsfdf2_helper(&res.i, a.i);

  return res.f;
}

float __truncdfsf2 (double A)
{
  double_union_t a;
  float_union_t res;

  a.f = A;
  res.i = __truncdfsf2_helper(&a.i);

  return res.f;
}

int __fixsfsi(float _a)
{
  float_union_t a;

  a.f = _a;

  return __fixsfsi_helper(a.i);
}

int __fixdfsi (double A)
{
  double_union_t a;

  a.f = A;

  return __fixdfsi_helper( &a.i );
}

long __fixsfdi (float A)
{
  /* sizeof(long) == sizeof(int) */
  return (long)__fixsfsi(A);
}

long __fixdfdi (double A)
{
  return (long)__fixdfsi(A);
}


unsigned int __fixunssfsi (float A)
{
  float_union_t a;

  a.f = A;

  return __fixunssfsi_helper(a.i);
}

unsigned int __fixunsdfsi (double A)
{
  double_union_t a;

  a.f = A;

  return __fixunsdfsi_helper(&a.i);
}


unsigned long __fixunssfdi (float A)
{
  return (unsigned long)__fixunssfsi(A);
}

unsigned long __fixunsdfdi (double A)
{
  return (unsigned long)__fixunsdfsi(A);
}


float __floatsisf(int i)
{
  float_union_t res;

  res.i = __floatsisf_helper(i);

  return res.f;
}

double __floatsidf (int I)
{
  double_union_t res;

  __floatsidf_helper(&res.i, I);

  return res.f;
}

float __floatdisf (long I)
{
  return __floatsisf((int)I);
}

double __floatdidf (long I)
{
  return __floatsidf((int)I);
}


int __cmpsf2 (float A, float B)
{
  float_union_t a, b;

  a.f = A; b.f = B;

  return __cmpsf2_helper(a.i, b.i);
}


int __cmpdf2 (double A, double B)
{
  double_union_t a, b;

  a.f = A; b.f = B;

  return __cmpdf2_helper(&a.i, &b.i);
}


int __unordsf2 (float A, float B)
{
  float_union_t a, b;

  a.f = A; b.f = B;
  if (__isnanf_helper(a.i) || __isnanf_helper(b.i))
    return 1;

  return 0;
}

int __unorddf2 (double A, double B)
{
  double_union_t a, b;

  a.f = A; b.f = B;
  if (__isnan_helper(&a.i) || __isnan_helper(&b.i))
    return 1;

  return 0;
}

/* See the gccint info page */
#define FLOAT_CMP_IMPLEMENTATION(name, E) \
int name(float A, float B)                \
{                                         \
  if (__unordsf2 (A, B))                  \
    return E;                             \
  return __cmpsf2 (A, B);                 \
}

#define DOUBLE_CMP_IMPLEMENTATION(name, E) \
int name(double A, double B)              \
{                                         \
  if (__unorddf2 (A, B))                  \
    return E;                             \
  return __cmpdf2 (A, B);                 \
}

FLOAT_CMP_IMPLEMENTATION(__eqsf2, 1);
DOUBLE_CMP_IMPLEMENTATION(__eqdf2, 1);

FLOAT_CMP_IMPLEMENTATION(__nesf2, 1);
DOUBLE_CMP_IMPLEMENTATION(__nedf2, 1);

FLOAT_CMP_IMPLEMENTATION(__gesf2, 0);
DOUBLE_CMP_IMPLEMENTATION(__gedf2, 0);

FLOAT_CMP_IMPLEMENTATION(__ltsf2, -1);
DOUBLE_CMP_IMPLEMENTATION(__ltdf2, -1);

FLOAT_CMP_IMPLEMENTATION(__lesf2, 1);
DOUBLE_CMP_IMPLEMENTATION(__ledf2, 1);

FLOAT_CMP_IMPLEMENTATION(__gtsf2, 0);
DOUBLE_CMP_IMPLEMENTATION(__gtdf2, 0);


/* Maybe implement these? */
float __floattisf (long long I);
double __floattidf (long long I);
long long __fixsfti (float A);
long long __fixdfti (double A);
unsigned long long __fixunssfti (float A);
unsigned long long __fixunsdfti (double A);

/* Won't be implemented - we don't support long doubles */
unsigned long __fixunstfdi (long double A);
unsigned long __fixunsxfdi (long double A);
long double __addtf3 (long double A, long double B);
long double __addxf3 (long double A, long double B);
long double __subtf3 (long double A, long double B);
long double __subxf3 (long double A, long double B);
long double __multf3 (long double A, long double B);
long double __mulxf3 (long double A, long double B);
long double __divtf3 (long double A, long double B);
long double __divxf3 (long double A, long double B);
long double __negtf2 (long double A);
long double __negxf2 (long double A);
long double __extendsftf2 (float A);
long double __extendsfxf2 (float A);
long double __extenddftf2 (double A);
long double __extenddfxf2 (double A);
double __truncxfdf2 (long double A);
double __trunctfdf2 (long double A);
float __truncxfsf2 (long double A);
float __trunctfsf2 (long double A);
int __fixtfsi (long double A);
int __fixxfsi (long double A);
long __fixtfdi (long double A);
long __fixxfdi (long double A);
long long __fixtfti (long double A);
long long __fixxfti (long double A);
unsigned int __fixunstfsi (long double A);
unsigned int __fixunsxfsi (long double A);
unsigned long long __fixunstfti (long double A);
unsigned long long __fixunsxfti (long double A);
long double __floatsitf (int I);
long double __floatsixf (int I);
long double __floatditf (long I);
long double __floatdixf (long I);
long double __floattitf (long long I);
long double __floattixf (long long I);
int __cmptf2 (long double A, long double B);
int __unordtf2 (long double A, long double B);
int __eqtf2 (long double A, long double B);
int __netf2 (long double A, long double B);
int __getf2 (long double A, long double B);
int __lttf2 (long double A, long double B);
int __letf2 (long double A, long double B);
int __gttf2 (long double A, long double B);


/* Redefine printf with %f support */
#define FLOAT_SUPPORT 1
#include "../libc/vsnprintf.c"
#undef FLOAT_SUPPORT

/* Define atof here to avoid needing softfloat from ANSI */
float atof(const char *nptr)
{
  int negative = 0;
  float num = 0;

  while (isspace(*nptr)) nptr++;
  switch (*nptr) {
    case '-':
      negative = 1;
    case '+':
      nptr++;
  }

  while (isdigit(*nptr)) {
    num *= 10;
    num += *nptr - '0';
    nptr++;
  }

  if (*nptr == '.') {
    nptr++;
    float fraction = 0.1;
    while (isdigit(*nptr)) {
      num += (*nptr - '0') * fraction;
      fraction *= 0.1;
      nptr++;
    }
  }

  if (negative) return -num;
  else return num;
}

