/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
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

#include <cibyl.h>

typedef int time_t;

time_t time(time_t* t); /* Not generated */

#if defined(__cplusplus)
}
#endif
#endif /* !__TIME_H__ */
