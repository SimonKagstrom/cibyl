/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      java-lang.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   java.lang syscalls
 *
 * $Id: lang.h 12060 2006-11-14 07:06:01Z ska $
 *
 ********************************************************************/
#ifndef __JAVA_LANG_H__
#define __JAVA_LANG_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>
#include <stdint.h>
#include "io.h"

typedef int NOPH_Object_t;
typedef int NOPH_Class_t;
typedef int NOPH_String_t;
typedef int NOPH_Throwable_t;

void NOPH_delete(NOPH_Object_t obj); /* Not generated */

/**
 * Write a Java String object to a char pointer.
 *
 * @param obj the String object to write
 * @param addr the address to write to
 * @param maxlen the length of the passed address
 */
void NOPH_String_toCharPtr(NOPH_String_t obj, char* addr, int maxlen); /* Not generated */

/* Should return a 64-bit value - we get wraparound problems otherwise! */
void __NOPH_System_currentTimeMillis(int64_t* p); /* Not generated */
static inline int64_t NOPH_System_currentTimeMillis(void)
{
  int64_t out;
  __NOPH_System_currentTimeMillis(&out);

  return out;
}
void NOPH_Thread_sleep(int ms); /* Throws */

void NOPH_Throwable_printStackTrace(NOPH_Throwable_t th);
NOPH_String_t NOPH_Throwable_getMessage(NOPH_Throwable_t th);
NOPH_String_t NOPH_Throwable_toString(NOPH_Throwable_t th);

/* Exception */
NOPH_Exception_t NOPH_Exception_new(void);
NOPH_Exception_t NOPH_Exception_new_string(char* s);

extern NOPH_Exception_t NOPH_Exception_new_string_va(char *s, ...);
#define NOPH_Exception_new_string NOPH_Exception_new_string_va

/* Object */
NOPH_Class_t NOPH_Object_getClass(NOPH_Object_t obj);

/* Class */
NOPH_InputStream_t NOPH_Class_getResourceAsStream(NOPH_Class_t obj, char* name); /* Throws */
NOPH_String_t NOPH_Class_getName(NOPH_Class_t obj);

#if defined(__cplusplus)
}
#endif
#endif /* !__JAVA_LANG_H__ */
