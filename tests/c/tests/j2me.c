/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      j2me.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   J2ME-specific tests
 *
 * $Id:$
 *
 ********************************************************************/
#include <java/lang.h>
#include <test.h>

static void System_currentTimeMillis(void)
{
  long long time = NOPH_System_currentTimeMillis();

  if (time > 0)
    PASS("System.currentTimeMillis > 0: %lld\n", time);
  else
    FAIL("System.currentTimeMillis < 0: %lld\n", time);
}

/* The run-the-tests function */
void j2me_run(void)
{
  System_currentTimeMillis();
}
