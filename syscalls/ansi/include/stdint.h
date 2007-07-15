/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      stdint.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   stdint for Cibyl
 *
 * $Id: stdint.h 12571 2006-11-29 15:21:40Z ska $
 *
 ********************************************************************/
#ifndef __STDINT_H__
#define __STDINT_H__

/* Explicitly sized types for MIPS1/Java */
typedef signed char      int8_t;
typedef unsigned char   uint8_t;
typedef short           int16_t;
typedef unsigned short uint16_t;
typedef int             int32_t;
typedef unsigned int   uint32_t;
typedef long long       int64_t;
typedef unsigned long long uint64_t;

#endif /* !__STDINT_H__ */
