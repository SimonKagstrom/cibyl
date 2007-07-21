/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      relocation.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Relocation tests
 *
 * $Id: relocation.c 13859 2007-02-25 15:50:13Z ska $
 *
 ********************************************************************/
#include <test.h>


int relocation_a(void)
{
  return 'a';
}

static int relocation_b(void)
{
  return 'b';
}

static int relocation_c(void)
{
  return 'c';
}

static int relocation_d(void)
{
  return 'd';
}

static int relocation_e(void)
{
  return 'e';
}

int relocation_f(void)
{
  return 'f';
}

typedef int (*fn_t)(void);

const fn_t relocation_fns[] =
{
  relocation_e,
  relocation_f,
};

int (*relocation_fn)(void) = relocation_a;

static int (*relocation_fn2)(void);
static int (*relocation_fn3)(void) = relocation_d;

static int trampoline(fn_t fn)
{
  return fn();
}

/* The run-the-tests function */
void relocation_run(void)
{
  static int (*relocation_fn4)(void) = relocation_a;
  int (*relocation_fn5)(void) = relocation_b;
  int v;

  /* Test a function call to a non-static method */
  if ( (v = relocation_fn()) != 'a')
    FAIL("Non-static fnptr call: %c", v);
  else
    PASS("Non-static fnptr call: %c", v);

  /* Reassign and assign to a static */
  relocation_fn = relocation_b;
  relocation_fn2 = relocation_c;

  /* Test a function call to a static method */
  if ( (v = relocation_fn()) != 'b')
    FAIL("Static fnptr call: %c", v);
  else
    PASS("Static fnptr call: %c", v);

  /* Test a function call to a static method */
  if ( (v = relocation_fn2()) != 'c')
    FAIL("Static fnptr call (reassign): %c", v);
  else
    PASS("Static fnptr call (reassign): %c", v);

  if ( (v = relocation_fn3()) != 'd')
    FAIL("Data fnptr call: %c", v);
  else
    PASS("Data fnptr call: %c", v);

  if ( (v = relocation_fn4()) != 'a')
    FAIL("Local static fnptr call (reassign): %c", v);
  else
    PASS("Local static fnptr call (reassign): %c", v);

  if ( (v = relocation_fn5()) != 'b')
    FAIL("Local fnptr call (reassign): %c", v);
  else
    PASS("Local fnptr call (reassign): %c", v);


  if ( (v = trampoline(relocation_fns[1])) != 'f')
    FAIL("Rodata call: %c", v);
  else
    PASS("Rodata call: %c", v);
}
