/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      ansi.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Implementation of some ANSI stuff
 *
 * $Id$
 *
 ********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <java/io.h>
#include <cibyl.h>

FILE *stdin; /* Does not exist in MIDP */
FILE *stdout;
FILE *stderr;

static void __attribute__((constructor)) setup_ansi_support(void)
{
  NOPH_OutputStream_t os_stdout;
  NOPH_OutputStream_t os_stderr;

  __setup_io((void*)&os_stdout, (void*)&os_stderr);
  stdout = NOPH_OutputStream_createFILE(os_stdout);
  stderr = NOPH_OutputStream_createFILE(os_stderr);
}

#undef putchar
int putchar(int c) { return __putchar(c); }

/* We need this to handle inlined puts (with __puts) and automatically
 * generated puts from printfs correctly
 */
#undef puts
int puts(const char* string) {return __puts(string);}

#undef fputs
int fputs(const char* ptr, FILE* stream) { return __fputs(ptr, stream); } /* Not generated */

int tolower (int c)
{
  if (c >= 'A' && c <= 'Z')
    return c + 'a' - 'A';
  else
    return c;
}

int toupper (int c)
{
  if (c >= 'a' && c <= 'z')
    return c - 'a' - 'A';
  else
    return c;
}

char *strstr(const char *haystack, const char *needle)
{
  if ((haystack == NULL) || (needle == NULL))
  {
    return NULL;
  }
  else
  {
    int i;
    int h_len = strlen (haystack);
    int n_len = strlen (needle);

    for (i = 0; i < (h_len - n_len + 1); i++) {
      if (!__strncmp(haystack + i, needle, n_len))
      {
        return (char *)(haystack + i);
      }
    }
  }

  return NULL;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
  const char *s = s1;
  const char *d = s2;
  int a = 0;
  int b = 0;

  do
    {
      if (s - s1 == n)
	break;

      a = tolower(*s);
      b = tolower(*d);

      s++;
      d++;
    } while ( a == b && b != 0 && a != 0 );

  if (a < b)
    return -1;
  else if (a > b)
    return 1;
  return 0;
}


/*
 * From FreeBSD libkern
 *
 * Shift a (signed) quad value left (arithmetic shift left).
 * This is the same as logical shift left!
 */
typedef long long quad_t;
typedef unsigned long long u_quad_t;
typedef unsigned int qshift_t;

#define CHAR_BIT 8
#define QUAD_BITS (sizeof(quad_t) * CHAR_BIT)
#define LONG_BITS (sizeof(long) * CHAR_BIT)
#define _QUAD_HIGHWORD 0 /* Big-endian */
#define _QUAD_LOWWORD 1
#define H _QUAD_HIGHWORD
#define L _QUAD_LOWWORD

union uu {
  quad_t q;/* as a (signed) quad */
  quad_t uq;/* as an unsigned quad */
  long sl[2];/* as two signed longs */
  unsigned long ul[2];/* as two unsigned longs */
};

quad_t
__ashldi3(a, shift)
     quad_t a;
     qshift_t shift;
{
  union uu aa;

  aa.q = a;
  if (shift >= LONG_BITS) {
    aa.ul[H] = shift >= QUAD_BITS ? 0 :
      aa.ul[L] << (shift - LONG_BITS);
    aa.ul[L] = 0;
  } else if (shift > 0) {
    aa.ul[H] = (aa.ul[H] << shift) |
      (aa.ul[L] >> (LONG_BITS - shift));
    aa.ul[L] <<= shift;
  }
  return (aa.q);
}
