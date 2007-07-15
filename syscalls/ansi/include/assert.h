/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      assert.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl stdlib stuff
 *
 * $Id: assert.h 12045 2006-11-13 19:58:48Z ska $
 *
 ********************************************************************/
#ifndef __ASSERT_H__
#define __ASSERT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#if defined(NDEBUG)
# define assert(x)
#else
# include <stdio.h>
# include <stdlib.h>
# define assert(x) do { \
 if ( ! (x) ) \
 {\
     printf("ASSERTION FAILED at %d in %s:%s\n", __LINE__, __FILE__, __FUNCTION__); \
     exit(1); \
 } \
 } while(0)
#endif

#if defined(__cplusplus)
}
#endif
#endif /* !__ASSERT_H__ */
