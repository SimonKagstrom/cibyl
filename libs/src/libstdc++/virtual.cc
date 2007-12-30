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

extern "C" void __cxa_atexit(void (*arg1)(void*), void* arg2, void* arg3)
{
}

extern "C" {
void*  __dso_handle = (void*) &__dso_handle;
}

typedef int __guard __attribute__((mode (__DI__)));

extern "C"
int __cxa_guard_acquire (__guard *g)
{
    return !*(char *)(g);
}

extern "C"
void __cxa_guard_release (__guard *g)
{
    *(char *)g = 1;
}

extern "C"
void __cxa_guard_abort (__guard *)
{
}
