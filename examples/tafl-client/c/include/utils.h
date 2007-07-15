/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      utils.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Utility defs
 *
 * $Id: utils.h 13453 2007-02-05 16:28:37Z ska $
 *
 ********************************************************************/
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

#include "config.h"

#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )

#define FATAL_ON_EXCEPTION(x) do { \
  (x); \
  if (NOPH_exception) { \
    NOPH_Throwable_printStackTrace(NOPH_exception); \
    exit(1); \
  } \
} while(0)

void int_to_charbuf(unsigned int val, char *buf, int digits);
int charbuf_to_int(char *buf, int digits);

#endif /* !__UTILS_H__ */
