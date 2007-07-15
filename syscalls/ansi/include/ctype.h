/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      ctype.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl ctype
 *
 * $Id: ctype.h 12096 2006-11-14 15:49:07Z ska $
 *
 ********************************************************************/
#ifndef __CTYPE_H__
#define __CTYPE_H__
#if defined(__cplusplus)
extern "C" {
#endif

static inline int isascii(int c)
{
  return ( (c & ~0x7f) == 0 );
}

static inline int isupper(int c)
{
  return ( c >= 'A' && c <= 'Z' );
}

static inline int islower(int c)
{
  return ( c >= 'a' && c <= 'z' );
}

static inline int isspace(int c)
{
  return (c == ' ') || (c == '\f') || (c == '\n') || (c == '\r') ||
         (c == '\t') || (c == '\v');
}

static inline int isdigit(int c)
{
  return (c >= '0') && (c <= '9');
}

extern int tolower (int c);
extern int toupper (int c);

#if defined(__cplusplus)
}
#endif
#endif /* !__CTYPE_H__ */
