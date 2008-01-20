/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      malloc.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Malloc test
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdlib.h>
#include <test.h>

static char *stack_end;

void malloc_test_heap_growth(void)
{
  size_t size = 7000;
  void *p = NULL;

  while ( 1 )
    {
      void *p2;
      char *end = p;

      end += size;

      if (end >= stack_end)
	FAIL("Heap and stack overlap: %x vs %x\n", end, stack_end);
      else
	PASS("No heap and stack overlap: %x vs %x\n", end, stack_end);

      size += 7000;
      p2 = realloc(p, size);
      if (!p2)
	{
	  free(p);
	  PASS("Malloc out-of-memory: %d\n", size);
	  return;
	}
      p = p2;
    }
}


/* The run-the-tests function */
void malloc_run(void)
{
  stack_end = (char*)((*(uint32_t*)12) - (*(uint32_t*)4));
  malloc_test_heap_growth();
}
