/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      time.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   time header file
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __SYS__TIME_H__
#define __SYS__TIME_H__

typedef long suseconds_t;
typedef int time_t;

struct timeval {
  time_t      tv_sec;     /* seconds */
  suseconds_t tv_usec;    /* microseconds */
};

struct timezone {
#if 0 /* Not yet supported */
  int tz_minuteswest;     /* minutes west of Greenwich */
  int tz_dsttime;         /* type of DST correction */
#endif
};


#endif /* !__SYS__TIME_H__ */
