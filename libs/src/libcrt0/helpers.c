/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      helpers.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   C runtime helpers
 *
 * $Id: helpers.c 13852 2007-02-24 07:26:18Z ska $
 *
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cibyl.h>

#include <java/lang.h>

extern unsigned long __ctors_begin;
extern unsigned long __ctors_end;
extern unsigned long __dtors_begin;
extern unsigned long __dtors_end;

static void atexit_run(void);

static void run_list(unsigned long *start, unsigned long *end)
{
  void (**p)(void) = (void (**)(void))start;

  while (p != (void (**)(void))end)
    {
      (*p)();
      p++;
    }
}

void crt0_run_global_constructors(void)
{
  run_list(&__ctors_begin, &__ctors_end);
  NOPH_registerCallback("Cibyl.atexit", (int)atexit_run);
}

void crt0_run_global_destructors(void)
{
  run_list(&__dtors_begin, &__dtors_end);
}

/* This is a quite common case for an exception handler */
void NOPH_setter_exception_handler(NOPH_Exception_t ex, void *arg)
{
  int *p = (int*)arg;
  *p = 1;

  NOPH_delete(ex);
}

NOPH_Exception_t NOPH_Exception_new_string_va(char *fmt, ...)
{
  char buf[255];
  va_list ap;

  /*
   * Print into buffer.
   */
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

#undef NOPH_Exception_new_string
  return NOPH_Exception_new_string(buf);
}


void NOPH_panic(const char *fmt, ...)
{
  char buf[255];
  va_list ap;

  /*
   * Print into buffer.
   */
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  fprintf(stderr, "PANIC: ");
  fprintf(stderr, buf);
  fprintf(stderr, "\n");
  NOPH_throw(NOPH_Exception_new());
  exit(1);
}

void NOPH_panic_if(int cond, const char *fmt, ...)
{
  char buf[255];
  va_list ap;

  if (!cond)
    return;

  /*
   * Print into buffer.
   */
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  fprintf(stderr, "PANIC: ");
  fprintf(stderr, buf);
  fprintf(stderr, "\n");
  NOPH_throw(NOPH_Exception_new());
  exit(1);
}

/* Dummy functions - handled by builtins always */
void __NOPH_try(void (*callback)(NOPH_Exception_t exception, void *arg), void *arg)
{
}

void __NOPH_catch(void)
{
}

void __NOPH_throw(NOPH_Exception_t ex)
{
}


/* --- atexit(3) handling */
static void (**atexit_list)(void);
static int atexit_n = 0;

/* atexit implementation */
int atexit( void (*fn)(void) )
{
  int cur = atexit_n;

  atexit_n++;
  atexit_list = realloc(atexit_list, sizeof(void (*)(void)) * atexit_n);
  atexit_list[cur] = fn;
  NOPH_panic_if(!atexit_list, "realloc of atexit lists failed");

  return 0;
}

static void atexit_run(void)
{
  unsigned long *start = (unsigned long*)atexit_list;
  unsigned long *end = start + atexit_n;

  run_list(start, end);

  crt0_run_global_destructors();
}
