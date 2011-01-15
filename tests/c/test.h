/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      test.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Test harness stuff
 *
 * $Id: test.h 13576 2007-02-10 15:31:09Z ska $
 *
 ********************************************************************/
#ifndef __TEST_H__
#define __TEST_H__

#include "console.h"

#ifndef VERBOSE
# define VERBOSE 0
#endif

#include <stdio.h>
#define __USE_BSD /* M_PI */
#include <math.h>

#define __binary_arithmetic_test(name, type_a, type_b, type_res, op) \
type_res name(type_a a, type_b b) \
{ \
  return ((a) op (b)); \
}

#define __binary_arithmetic_fn_test(name, type_a, type_b, type_res, fn) \
type_res name(type_a a, type_b b) \
{ \
  return fn((a), (b)); \
}

#define __binary_arithmetic_test_constant(name, type_a, b, type_res, op) \
type_res name(type_a a) \
{ \
  return ((a) op (b)); \
}

#define __unary_arithmetic_test(name, type_a, type_res, op) \
type_res name(type_a a) \
{ \
  return op (a);        \
}

#define typecast_test(name, type_a, type_b)      \
type_a name(type_a val)                          \
{                                                \
  return (type_a)(type_b)val;                    \
}

#define bin_test_struct(_name, type)             \
typedef struct                                   \
{                                                \
  type (*fn)(type a, type b);                    \
  const char *name;                              \
  type a;                                        \
  type b;                                        \
  type expected_res;                             \
} _name;

#define un_test_struct(_name, type)              \
typedef struct                                   \
{                                                \
  type (*fn)(type a);                            \
  const char *name;                              \
  type a;                                        \
  type expected_res;                             \
} _name;


#define binary_arithmetic_test_char(name, op) __binary_arithmetic_test(name, char, char, char, op)
#define binary_arithmetic_test_char_constant(name, constant, op) __binary_arithmetic_test(name, char, constant, char, op)
#define binary_arithmetic_test_short(name, op) __binary_arithmetic_test(name, short, short, short, op)
#define binary_arithmetic_test_short_constant(name, constant, op) __binary_arithmetic_test(name, short, constant, short, op)
#define binary_arithmetic_test_int(name, op) __binary_arithmetic_test(name, int, int, int, op)
#define binary_arithmetic_test_int_constant(name, constant, op) __binary_arithmehic_test(name, int, constant, int, op)
#define binary_arithmetic_test_longlong(name, op) __binary_arithmetic_test(name, long long, long long, long long, op)
#define binary_arithmetic_test_longlong_constant(name, constant, op) __binary_arithmehic_test(name, long long, constant, long long, op)

#define binary_arithmetic_test_unsigned_char(name, op) __binary_arithmetic_test(name, unsigned char, unsigned char, unsigned char, op)
#define binary_arithmetic_test_unsigned_char_constant(name, constant, op) __binary_arithmetic_test(name, unsigned char, constant, unsigned char, op)
#define binary_arithmetic_test_unsigned_short(name, op) __binary_arithmetic_test(name, unsigned short, unsigned short, unsigned short, op)
#define binary_arithmetic_test_unsigned_short_constant(name, constant, op) __binary_arithmetic_test(name, unsigned short, constant, unsigned short, op)
#define binary_arithmetic_test_unsigned_int(name, op) __binary_arithmetic_test(name, unsigned int, unsigned int, unsigned int, op)
#define binary_arithmetic_test_unsigned_int_constant(name, constant, op) __binary_arithmehic_test(name, unsigned int, constant, unsigned int, op)

#define binary_arithmetic_test_float(name, op) __binary_arithmetic_test(name, float, float, float, op)
#define binary_arithmetic_test_float_constant(name, constant, op) __binary_arithmetic_test(name, float, constant, float, op)
#define binary_arithmetic_test_double(name, op) __binary_arithmetic_test(name, double, double, double, op)
#define binary_arithmetic_test_double_constant(name, constant, op) __binary_arithmetic_test(name, double, constant, double, op)

#define binary_arithmetic_test_fn_float(name, op) __binary_arithmetic_fn_test(name, float, float, float, op)
#define binary_arithmetic_test_fn_double(name, op) __binary_arithmetic_fn_test(name, double, double, double, op)

#define unary_arithmetic_test_float(name, op) __unary_arithmetic_test(name, float, float, op)
#define unary_arithmetic_test_double(name, op) __unary_arithmetic_test(name, double, double, op)

/* Define a struct for tests */
bin_test_struct(char_test_t, char);
bin_test_struct(unsigned_char_test_t, unsigned char);
bin_test_struct(short_test_t, short);
bin_test_struct(unsigned_short_test_t, unsigned short);
bin_test_struct(int_test_t, int);
bin_test_struct(longlong_test_t, long long);
bin_test_struct(unsigned_int_test_t, unsigned int);

bin_test_struct(float_test_t, float);
bin_test_struct(double_test_t, double);

un_test_struct(char_un_test_t, char);
un_test_struct(unsigned_char_un_test_t, unsigned char);
un_test_struct(short_un_test_t, short);
un_test_struct(unsigned_short_un_test_t, unsigned short);
un_test_struct(int_un_test_t, int);
un_test_struct(longlong_un_test_t, long long);
un_test_struct(unsigned_int_un_test_t, unsigned int);

un_test_struct(double_un_test_t, double);
un_test_struct(float_un_test_t, float);

/* One binary operation */
#define BIN_OP(name, op, a, b) { name, #name ": " #a " " #op " " #b, a, b, (a) op (b)}
#define BIN_OP_UNSIGNED(name, op, a, b) { name, #name ": " #a " " #op " " #b, a, b, ((unsigned)a) op ((unsigned)b)}
#define FN_BIN_OP(name, op, a, b, res) { name, #name ": " #a " " #op " " #b, a, b, res}

#define UN_OP(name, op, a) { name, #name ": " #op "" #a, a, op (a)}
#define TC_OP(name, type_a, type_b, a) { name, #name ": ("#type_a ")(" #type_b ")(" #a ")", a, (type_a)(type_b) (a)}
#define FN_OP(name, op, a, res) { name, #name ": " #op "(" #a ")", a, res}

extern int test_output(const char *fmt, ...);

#define FAIL(x, y...)                         \
     do {                                     \
       if (do_output) test_output("FLD: " x, y); \
       n_fail++;                              \
     } while(0)

#define PASS(x, y...)                                 \
     do {                                             \
       if (do_output && verbose) test_output("PSS: " x, y); \
       n_pass++;                                      \
     } while(0)


#define run_test_bin_vector(data_type, type, __name, fmt) {\
  type *tst = __name; \
                                                  \
  for (tst = __name;                              \
       tst != &__name[sizeof(__name) / sizeof(type)]; \
       tst++)                                     \
    {                                             \
      data_type res = tst->fn( tst->a, tst->b );  \
                                                  \
      if ( res != tst->expected_res &&            \
	   (!(isnan(res) && isnan(tst->expected_res))) ) \
	FAIL("%s: " fmt " != " fmt "\n", tst->name, res, tst->expected_res); \
      else                                                                   \
	PASS("%s: " fmt " == " fmt "\n", tst->name, res, tst->expected_res); \
    }                                                                        \
  }

#define run_test_un_vector(data_type, type, __name, fmt) {\
  type *tst = __name; \
                                                  \
  for (tst = __name;                              \
       tst != &__name[sizeof(__name) / sizeof(type)]; \
       tst++)                                     \
    {                                             \
      data_type res = tst->fn( tst->a );          \
                                                  \
      if ( res != tst->expected_res &&            \
	   (!(isnan(res) && isnan(tst->expected_res))) ) \
	FAIL("%s: " fmt " != " fmt "\n", tst->name, res, tst->expected_res); \
      else                                                                   \
	PASS("%s: " fmt " == " fmt "\n", tst->name, res, tst->expected_res); \
    }                                                                        \
  }


extern int verbose;
extern int do_output;
extern int n_fail;
extern int n_pass;

#endif /* !__TEST_H__ */
