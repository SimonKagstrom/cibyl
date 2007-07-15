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

/* The run-the-tests function */
void exceptions_run(void)
{
  char *s = "http://spel.bth.se/~ska/index.html";

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
