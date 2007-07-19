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
#include <test.h>
#include <stdlib.h>
#include <javax/microedition/io.h>
#include <javax/microedition/lcdui/game.h>

static int exceptions_a;
int exceptions_b;
int exceptions_c;
int caught_exception;

static void handler(NOPH_Exception_t exception)
{
  caught_exception = 1;
  if (exception > 100000)
    FAIL("Exception number: %x", exception);
  else
    PASS("Exception number: %x", exception);
}

void exception_test_1(void)
{
  int i;

  exceptions_a = 1;
  exceptions_b = 10;
  caught_exception = 0;

  NOPH_try(handler)
    {
      for (i = 10; i >= 0; i--)
        exceptions_c = exceptions_b / i;
      exceptions_b = exceptions_a;
    } NOPH_catch();
  if (!caught_exception)
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

  NOPH_try(handler)
    {
      int w = NOPH_GameCanvas_getWidth(-1); /* Should throw an exception */
      exceptions_b = w;
    } NOPH_catch();
  if (!caught_exception)
    FAIL("array-idx-out exception: %d", caught_exception);
  else
    PASS("array-idx-out excption %d", caught_exception);

  if (exceptions_b != 10)
    FAIL("executing code after exception: %d != %d", exceptions_b, exceptions_a);
  else
    PASS("executing code after exception: %d != %d", exceptions_b, exceptions_a);
}


/* The run-the-tests function */
void exceptions_run(void)
{
  char *s = "http://spel.bth.se/~ska/index.html";

  exceptions_a = 0;
  exceptions_b = 0;
  exceptions_c = 0;

  exception_test_1();
  exception_test_2();

  NOPH_Connector_openDataInputStream(s);
  if (NOPH_exception)
    FAIL("opening %s should not throw an exception\n", s);
  else
    PASS("opening %s did not throw an exception\n", s);

  s = "bttp://spel.bth.se/~ska/index.html"; /* hopefully never a protocol :-) */
  NOPH_Connector_openDataInputStream(s);
  if (!NOPH_exception)
    FAIL("opening %s should throw an exception\n", s);
  else
    PASS("opening %s threw an exception\n", s);
}
