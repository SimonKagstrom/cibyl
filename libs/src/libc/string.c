/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      string.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   string.h ANSI C functionality
 *
 * $Id: string.c 13453 2007-02-05 16:28:37Z ska $
 *
 ********************************************************************/
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define INT_MAX 2147483647

void *memset(void *s, int c, size_t n)
{
  char *p = s;

  if ( n > sizeof(int) )
    {
      while ( ((unsigned long)p & (sizeof(int)-1)) != 0 && n > 0 )
        {
          *p = c;
    	  p++;
	  n--;
        }

      /* Middle */
      int i_c = ( (c << 24) | (c << 16) | (c << 8) | c );
      while ( n > sizeof(int) )
        {
          int *p_i = (int*)p;
          *p_i = i_c;
          p += sizeof(int);
          n -= sizeof(int);
        }
    }

  /* Last */
  for ( ;
	n > 0;
	p++, n--)
    *p = c;

  return s;
}

/* Will be optimized away by GCC for the builtin cases */
void *memcpy(void *dest, const void *src, size_t n)
{
  char *d = (char*)dest;
  char *s = (char*)src;

  if ( ((((int)src) | ((int)dest) | n) & (sizeof(int)-1)) == 0 )
    {
      while ( n > 0 )
        {
          *(int*)d = *(int*)s;
          s += sizeof(int);
	  d += sizeof(int);
          n -= sizeof(int);
        }
      return dest;
    }

  while ( n > 0 )
    {
      *d = *s;
      d++;
      s++;
      n--;
    }

  return dest;
}

void *memmove(void *s1, const void *s2, size_t n)
{
  register char *s = (char *) s1;
  register const char *p = (const char *) s2;

  if (p >= s) {
    while (n) {
      *s++ = *p++;
      --n;
    }
  } else {
    while (n) {
      --n;
      s[n] = p[n];
    }
  }

  return s1;
}

/*
 * From uClibc, some modifications ...
 *
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
char *strtok_r(char *s1, const char *s2, char **next_start)
{
  char *s;
  char *p;

  if (((s = s1) != NULL) || ((s = *next_start) != NULL)) {
    if (*(s += strspn(s, s2))) {
      if ((p = strpbrk(s, s2)) != NULL) {
        *p++ = 0;
      }
    } else {
      p = s = NULL;
    }
    *next_start = p;
  }
  return s;
}

char *strtok(char *s1, const char *s2)
{
  static char *next_start;/* Initialized to 0 since in bss. */
  return strtok_r(s1, s2, &next_start);
}

char *strpbrk(const char *s1, const char *s2)
{
  register const char *s;
  register const char *p;

  for ( s=s1 ; *s ; s++ )
    {
      for ( p=s2 ; *p ; p++ )
        {
          if (*p == *s)
            return (char *) s; /* silence the warning */
        }
    }
  return NULL;
}

size_t strspn(const char *s1, const char *s2)
{
  register const char *s = s1;
  register const char *p = s2;

  while (*p)
    {
      if (*p++ == *s)
        {
          ++s;
          p = s2;
        }
    }
  return s - s1;
}

size_t strcspn(const char *s1, const char *s2)
{
  register const char *s;
  register const char *p;

  for ( s=s1 ; *s ; s++ )
    {
      for ( p=s2 ; *p ; p++ )
        {
          if (*p == *s)
            goto done;
        }
    }
 done:
  return s - s1;
}

char *strchr(const char *s, int c)
{
  do
    {
      if (*s == ((char)c))
        {
          return (char *) s;/* silence the warning */
        }
    } while (*s++);

  return NULL;
}

char *strrchr(register const  char *s, int c)
{
  const char *p;

  p = NULL;
  do
    {
      if (*s == (char) c)
        {
          p = s;
        }
    } while (*s++);

  return (char *) p;/* silence the warning */
}

/* ... until here */

char *strcat(char *dest, const char *src) { return __strcat(dest, src); }
char *strncat(char *dest, const char *src, size_t n) { return __strncat(dest, src, n); }
char *strncpy(char *dest, const char *src, size_t n) { return __strncpy(dest, src, n); }
int strcmp(const char* s1, const char* s2) { return __strcmp(s1, s2); }
int strncmp(const char *s1, const char *s2, size_t n) {return __strncmp(s1,s2,n); }
int strcasecmp(const char *s1, const char *s2) {return strncasecmp(s1,s2, INT_MAX); }

/*
 * Copyright (C) 2002     Manuel Novoa III
 * Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */
size_t strlen(const char *s)
{
  register const char *p;

  for (p=s ; *p ; p++);

  return p - s;
}

char *strcpy(char * s1, const char * s2)
{
  register char *s = s1;

  while ( (*s++ = *s2++) != 0 );

  return s1;
}

int memcmp(const void* s1, const void* s2, size_t n) { return __memcmp(s1, s2, n); }

char *strdup(const char *s) {
        char *str = (char *)malloc(strlen(s) + 1);
        if (!str)
          return NULL;
        strcpy(str, s);
        return str;
}
