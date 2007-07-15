/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      java-util.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   java.util syscalls
 *
 * $Id: $
 *
 ********************************************************************/
#ifndef __JAVA_UTIL_H__
#define __JAVA_UTIL_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <java/lang.h>

typedef int NOPH_Enumeration_t;

bool_t NOPH_Enumeration_hasMoreElements(NOPH_Enumeration_t enumeration);
NOPH_Object_t NOPH_Enumeration_nextElement(NOPH_Enumeration_t enumeration); /* Throws */

#if defined(__cplusplus)
}
#endif
#endif /* !__JAVA_UTIL_H__ */
