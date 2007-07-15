/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      varargs.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   varargs test
 *
 * $Id: varargs.c 13026 2007-01-15 12:23:58Z ska $
 *
 ********************************************************************/
#include <test.h>
#include <stdarg.h>

double scratch;

void varargs_self(int fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  scratch = (double)va_arg(ap, double);
  va_end(ap);
}

void varargs_pass_va_list(va_list ap)
{
  scratch = (double)va_arg(ap, double);
}

void varargs_chained(int fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  varargs_pass_va_list(ap);
  va_end(ap);
}


void varargs_run(void)
{
  double test_val = 1.9;

  scratch = 0.0;
  varargs_chained(0, test_val);
  if (scratch != test_val)
    FAIL("chained: %f != %f\n", scratch, test_val);
  else
    PASS("chained: %f == %f\n", scratch, test_val);


  scratch = 0.0;
  varargs_self(0, test_val);
  if (scratch != test_val)
    FAIL("unchained: %f != %f\n", scratch, test_val);
  else
    PASS("unchained: %f == %f\n", scratch, test_val);
}
