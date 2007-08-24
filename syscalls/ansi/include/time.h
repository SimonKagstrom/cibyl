/*********************************************************************
 *
 * Copyright (C) 2006-2007,  Simon Kagstrom
 *
 * Filename:      time.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Time defs
 *
 * $Id: time.h 13572 2007-02-10 10:44:29Z ska $
 *
 ********************************************************************/
#ifndef __TIME_H__
#define __TIME_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <sys/time.h>
#include <cibyl.h>

time_t time(time_t* t); /* Not generated */

int __gettimeofday(int* tv, int* tz); /* Not generated */

static inline int gettimeofday(struct timeval* tv, struct timezone* tz)
{
  return __gettimeofday((int*)tv, (int*)tz);
}

#if defined(__cplusplus)
}
#endif
#endif /* !__TIME_H__ */
