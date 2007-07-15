/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      config.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Platform config
 *
 * $Id: config.h 12173 2006-11-16 12:37:16Z ska $
 *
 ********************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#if defined(CIBYL)
#else
typedef int bool_t;
#endif

#define TRUE 1
#define FALSE 0

#endif /* !__CONFIG_H__ */
