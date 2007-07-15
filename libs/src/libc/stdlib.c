/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      stdlib.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl stdlib impl.
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

long int strtol(const char *nptr, char **endptr, int base)
{
  int negative = 0;
  int num = 0;

  assert(base == 10 || base == 16 || base == 0);

  while (isspace(*nptr))
    nptr++;
  switch (*nptr)
    {
    case '-':
      negative = 1;
    case '+':
      nptr++;
    }

  if ( (base == 0 || base == 16) && (nptr[0] == '0' && nptr[1] == 'x') )
    {
      base = 16;
      nptr += 2;
    }

  while (isdigit(*nptr) || (*nptr >= 'a' && *nptr < 'a' + (base-10)) || (*nptr >= 'A' && *nptr < 'A' + (base-10)) )
    {
      if (base == 10 && !isdigit(*nptr))
        break;

      num *= base;
      if ( *nptr >= 'a' && *nptr < 'a' + (base-10) )
        num += *nptr - 'a' + 10;
      else if ( *nptr >= 'A' && *nptr < 'A' + (base-10) )
        num += *nptr - 'A' + 10;
      else
        num += *nptr - '0';
      nptr++;
    }

  if (endptr)
    *endptr = (char *)nptr;

  if (negative)
    return -num;
  else
    return num;
}

unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
  /* FIXME: not quite correct */
  return (unsigned long int)strtol(nptr, endptr, base);
}

int atoi(const char *nptr)
{
  return strtol(nptr, NULL, 10);
}
