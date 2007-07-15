/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      virtual.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   C++ support, from http://www.osdev.org/wiki/C_PlusPlus
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdlib.h>
#include <stddef.h> /* size_t */

void * operator new (size_t size)
{
  return malloc(size);
}

void * operator new[] (size_t size)
{
  return malloc(size);
}

void operator delete (void *p)
{
  free(p);
}

void operator delete[] (void *p)
{
  free(p);
}
