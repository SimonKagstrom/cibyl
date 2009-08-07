/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      syscall.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Syscall defs
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __SYSCALL_HH__
#define __SYSCALL_HH__

#include <stdio.h>
#include <string.h>
#include <utils.h>

#define CIBYL_DB_QUALIFIER_THROWS         1
#define CIBYL_DB_QUALIFIER_NOT_GENERATED  2

#define CIBYL_DB_RETURN_VOID    0
#define CIBYL_DB_RETURN_INT     1
#define CIBYL_DB_RETURN_BOOLEAN 2
#define CIBYL_DB_RETURN_OBJREF  3

#define CIBYL_DB_ARG_OBJREF 1

typedef struct
{
  unsigned long flags;
  char *javaType;
  char *type;
  char *name;
} cibyl_db_arg_t;

typedef struct
{
  unsigned long nr;
  unsigned long returns;
  unsigned long nrArgs;
  unsigned long qualifier;
  char *name;
  char *javaClass;
  char *javaMethod;
  char *set; /* The "syscall set" this belongs to */
  char *returnType;
  cibyl_db_arg_t *args;
  unsigned long user;
} cibyl_db_entry_t;

class Syscall
{
public:
  Syscall(const char *name, int nrArguments, char returnValue);

  char *getJavaSignature() { return this->javaSignature; }

  bool returnsValue() { return this->returnValue != 'V'; };

  int getRegistersToPass() { return this->nrArguments; }

private:
  int nrArguments;
  char *javaSignature;
  char returnValue;
};

#endif /* !__SYSCALL_HH__ */
