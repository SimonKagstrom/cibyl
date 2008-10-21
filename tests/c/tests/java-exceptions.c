/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      java-exceptions.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Test of Java exceptions
 *
 * $Id: java-exceptions.c 13568 2007-02-10 10:23:08Z ska $
 *
 ********************************************************************/
#ifdef HOST
void exceptions_run(void)
{
}
#else
#include <test.h>
#include <stdlib.h>
#include <setjmp.h>
#include <javax/microedition/io.h>
#include <javax/microedition/lcdui/game.h>

static int exceptions_a;
int exceptions_b;
int exceptions_c;
int caught_exception;

static void handler(NOPH_Exception_t exception, void *arg)
{
  if ( (int)arg < 1 || (int)arg > 7)
    FAIL("Exception argument: %x", (int)arg);
  else
    PASS("Exception argument: %x", (int)arg);

  caught_exception++;
  if (exception > 100000)
    FAIL("Exception object: %d", exception);
  else
    PASS("Exception object: %d", exception);

  NOPH_delete(exception);
}

void exception_test_1(void)
{
  int i;

  exceptions_a = 1;
  exceptions_b = 10;
  caught_exception = 0;

  NOPH_try(handler, (void*)3)
    {
      for (i = 10; i >= 0; i--)
        exceptions_c = exceptions_b / i;
      exceptions_b = exceptions_a;
    } NOPH_catch();
  if (caught_exception != 1)
    FAIL("div-by-zero exception: %d", caught_exception);
  else
    PASS("div-by-zero excption %d", caught_exception);

  if (exceptions_b == 1)
    FAIL("executing code after exception: %d != %d", exceptions_b, exceptions_a);
  else
    PASS("executing code after exception: %d != %d", exceptions_b, exceptions_a);
}

void exception_test_2(void)
{
  exceptions_a = 1;
  exceptions_b = 10;
  caught_exception = 0;

  NOPH_try(handler, (void*)4)
    {
      int w = NOPH_GameCanvas_getWidth(-1); /* Should throw an exception */
      exceptions_b = w;
    } NOPH_catch();
  if (caught_exception != 1)
    FAIL("array-idx-out exception: %d", caught_exception);
  else
    PASS("array-idx-out exception: %d", caught_exception);

  if (exceptions_b != 10)
    FAIL("executing code after exception: %d != %d", exceptions_b, exceptions_a);
  else
    PASS("executing code after exception: %d != %d", exceptions_b, exceptions_a);
}

void exception_div_by_zero(void)
{
  int i;

  exceptions_a = 1;
  exceptions_b = 10;

  for (i = 10; i >= 0; i--)
    exceptions_c = exceptions_b / i;
  exceptions_b = exceptions_a;
}

void exception_div_by_zero_catch(void)
{
  NOPH_try(handler, (void*)7)
    {
      exception_div_by_zero();
    } NOPH_catch();
}

void exception_test_stacked(void)
{
  caught_exception = 0;

  NOPH_try(handler, (void*)5)
    {
      exception_div_by_zero_catch();
      /* Should be OK - we should get here */
      int w = NOPH_GameCanvas_getWidth(-1); /* Should throw an exception */
      exceptions_b = w;
    } NOPH_catch();


  if (caught_exception != 2)
    FAIL("stacked exceptions: %d", caught_exception);
  else
    PASS("stacked exceptions: %d", caught_exception);

  if (exceptions_b != 10)
    FAIL("executing code after stacked exception: %d != %d", exceptions_b, exceptions_a);
  else
    PASS("executing code after stacked exception: %d != %d", exceptions_b, exceptions_a);
}

void exception_test_multiple(void)
{
  caught_exception = 0;

  NOPH_try(handler, (void*)1)
    {
      exception_div_by_zero();
    } NOPH_catch();

  NOPH_try(handler, (void*)2)
    {
      int w = NOPH_GameCanvas_getWidth(-1); /* Should throw an exception */
      exceptions_b = w;
    } NOPH_catch();


  if (caught_exception != 2)
    FAIL("multiple exceptions: %d", caught_exception);
  else
    PASS("multiple excptions: %d", caught_exception);

  if (exceptions_b != 10)
    FAIL("code after multiple exceptions: %d != %d", exceptions_b, exceptions_a);
  else
    PASS("code after multiple exceptions: %d != %d", exceptions_b, exceptions_a);
}

static void handler_file_io(NOPH_Exception_t exception, void *arg)
{
  *(int*)arg = 1;
  NOPH_delete(exception);
}

jmp_buf jb;
int exc_dummy = 0;
void exception_test_longjmp_2(void)
{
  exc_dummy = 2;
  longjmp(jb, 2);
}

void exception_test_longjmp_1(void)
{
  exception_test_longjmp_2();
  /* Will never be executed */
  exc_dummy = 1;
}

void exception_test_setjmp(void)
{
  int v;

  exc_dummy = 0;
  v = setjmp(jb);

  if (v == 0)
    {
      PASS("setjmp: %d", v);
    }
  else if (v == 2)
    {
      PASS("after longjmp: %d", v);
      if (exc_dummy != 2)
        FAIL("longjmp executed too much code: %d", exc_dummy);
      else
        PASS("longjmp executed correct code: %d", exc_dummy);

      return;
    }
  else
    {
      FAIL("setjmp: %d", v);
      return;
    }

  exception_test_longjmp_1();
}

extern void assign_to_ra(void);

/* The run-the-tests function */
void exceptions_run(void)
{
  char *s = "http://spel.bth.se/~ska/index.html";
  int threw_exception;

  exceptions_a = 0;
  exceptions_b = 0;
  exceptions_c = 0;

  exception_test_1();
  exception_test_2();
  exception_test_multiple();
  exception_test_stacked();
  exception_test_setjmp();

  NOPH_try(handler_file_io, (void*)&threw_exception) {
    NOPH_Connector_openDataInputStream(s);
    threw_exception = 0;
  } NOPH_catch();
  if (threw_exception)
    FAIL("opening %s should not throw an exception\n", s);
  else
    PASS("opening %s did not throw an exception\n", s);

  assign_to_ra();
}
#endif
