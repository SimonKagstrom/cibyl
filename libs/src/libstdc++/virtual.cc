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
#include <stdio.h>

extern "C" void __cxa_pure_virtual()
{
  /* Error if a pure virtual function couldn't be called */
  printf("Pure virtual function called\n");
  exit(1);
}
