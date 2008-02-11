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

/* The syscall database. Everything in big-endian order */
typedef struct
{
  uint32_t nr;
  uint32_t returns;
  uint32_t nrArgs;
  uint32_t qualifier;
  char *name;
} cibyl_db_entry_t;

class Syscall
{
public:
  Syscall(const char *name, int nrArguments, char returnValue)
  {
    int len = strlen(name) + nrArguments + 4 + strlen("Syscalls/");
    int i, n;

    this->javaSignature = (char*)xcalloc( len, sizeof(char) );
    n = snprintf(this->javaSignature, len, "Syscalls/%s(", name);
    for (i = 0; i < nrArguments; i++)
      this->javaSignature[n + i] = 'I';
    this->javaSignature[n + i] = ')';
    i++;
    this->javaSignature[n + i] = returnValue;

    this->returnValue = returnValue;
  }

  char *getJavaSignature() { return this->javaSignature; }

  bool returnsValue() { return this->returnValue != 'V'; };

  int getRegistersToPass() { return this->nrArguments; }

private:
  int nrArguments;
  char *javaSignature;
  char returnValue;
};

#endif /* !__SYSCALL_HH__ */
