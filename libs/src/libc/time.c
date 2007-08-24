/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      time.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:
 *
 * $Id:$
 *
 ********************************************************************/
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <java/lang.h>

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  uint64_t tm = NOPH_System_getCurrentTimeMillis();

  tv->tv_sec = tm / 1000;
  tv->tv_sec = tm % 1000;

  return 0;
}
