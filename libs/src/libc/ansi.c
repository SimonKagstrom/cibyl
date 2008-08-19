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
