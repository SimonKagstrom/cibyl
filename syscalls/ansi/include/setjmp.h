/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      stdio.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Cibyl setjmp.h
 *
 * $Id: $
 *
 ********************************************************************/
#ifndef __SETJMP_H__
#define __SETJMP_H__

#include <cibyl.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef int NOPH_SetjmpException_t;

NOPH_SetjmpException_t NOPH_SetjmpException_new(int cookie, int val);
int NOPH_SetjmpException_getCookie(NOPH_SetjmpException_t e);
int NOPH_SetjmpException_getValue(NOPH_SetjmpException_t e);

typedef int jmp_buf[1];

/* "Builtins" */
extern int __NOPH_setjmp(int cookie);

static inline int __attribute__((returns_twice)) setjmp(jmp_buf env)
{
  int cookie;
  int out;

  asm(".pushsection .cibylsetjmp\n"
      "1:                             \n"
      ".long 2f                       \n"
      ".popsection                    \n"
      "    la   %[cookie], 1b         \n"
      "2:  jal __NOPH_setjmp          \n"
      "    move %[out], $2            \n"
      ""
      : [cookie]"=r"(cookie), [out]"=r"(out)
      :
      : "memory", "$2", "$4");
  env[0] = cookie;

  /* The longjmp return value will be "generated" */
  return out;
}

static inline void longjmp(jmp_buf env, int val)
{
  NOPH_SetjmpException_t e = NOPH_SetjmpException_new(env[0], val);

  /* Raise the exception */
  NOPH_throw(e);
}

#if defined(__cplusplus)
}
#endif

#endif
