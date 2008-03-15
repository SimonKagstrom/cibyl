/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      function.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Function call tests
 *
 * $Id:$
 *
 ********************************************************************/
#include <test.h>

/* The run-the-tests function */
void function_run(void)
{
#if 0
  implicit_declaration(1, 2, "tre", "fyra", "fem",
                       "sex", 7, 8, 9, "tio", "elva");
#endif
}

void implicit_declaration(unsigned a1, long long a2, char *a3, char *a4, char *a5,
                          char *a6, long a7, int a8, int a9, char *a10, char *a11)
{
#define TST_V(which, correct) do {                  \
  if (which != correct)                             \
    FAIL("impl declr %d != %d\n", which, correct ); \
  else                                              \
    PASS("impl declr %d == %d\n", which, correct ); \
  } while(0)

#define TST_STR(which) do {           \
  if (which != NULL)                  \
    PASS("impl declr %s\n", #which ); \
  else                                \
    FAIL("impl declr %s\n", #which ); \
  } while(0)

  TST_V(a1, 1);
  TST_V(a2, 2);
  TST_V(a7, 7);
  TST_V(a8, 8);
  TST_V(a9, 9);

  TST_STR(a3);
  TST_STR(a4);
  TST_STR(a5);
  TST_STR(a6);
  TST_STR(a10);
  TST_STR(a11);
}
