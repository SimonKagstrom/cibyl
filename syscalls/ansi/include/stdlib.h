/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      stdlib.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl
 *
 * $Id: stdlib.h 13521 2007-02-09 17:16:14Z ska $
 *
 ********************************************************************/
#ifndef __STDLIB_H__
#define __STDLIB_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>
#include <stddef.h>

/* Helper to register callback functions (this is not part of ANSI C, but... ) */
int NOPH_registerCallback(char* name, int fnAddr); /* Not generated */

void __exit(int code); /* Not generated */
extern void __attribute__((noreturn)) exit(int code);
#define abort() __exit(1)

extern int atexit(void (*function)(void));

extern void* malloc(size_t size);
extern void free(void* ptr);
extern long long int strtoll(const char *nptr, char **endptr, int base);
extern long int strtol(const char *nptr, char **endptr, int base);
extern unsigned long long int strtoull(const char *nptr, char **endptr, int base);
extern unsigned long int strtoul(const char *nptr, char **endptr, int base);
extern float atof(const char *nptr);
extern int atoi(const char *nptr);

int rand(void); /* Not generated */
void srand(unsigned int seed); /* Not generated */

#include <string.h>

static inline void *calloc(size_t nmemb, size_t size)
{
  void *out = (void*)malloc(nmemb * size);
  memset(out, 0, nmemb * size);

  return out;
}

extern void *realloc(void *ptr, size_t size);

static inline int abs(int x)
{
  if (x > 0) return x;
  else return -x;
}

#if defined(__cplusplus)
}
#endif
#endif /* !__STDLIB_H__ */
