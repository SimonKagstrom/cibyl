/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      cibyl.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl defs
 *
 * $Id: cibyl.h 13453 2007-02-05 16:28:37Z ska $
 *
 ********************************************************************/
#ifndef __CIBYL_H__
#define __CIBYL_H__

/* Stack size */
#ifndef NOPH_STACK_SIZE
# define NOPH_STACK_SIZE  8192 /* 8KB stack */
#endif

#ifndef __ASSEMBLER__

# define SECTION(x) __attribute__((section (x)))

/* Convenience */
typedef int bool_t;

/* Special object! */
typedef int NOPH_Exception_t;

/** Include the generated syscall definitions
 *
 * Thanks to Ian Lance Taylor for the use of .set push / .set pop.
 */
# include "cibyl-syscall_defs.h"

/** The exception variable is always in register k0 */
register NOPH_Exception_t NOPH_exception asm("$26");


extern void __NOPH_try(void (*callback)(NOPH_Exception_t exception));

#define NOPH_try(callback) do { \
  asm volatile("");             \
  __NOPH_try(callback);         \
} while(0); do

extern void __NOPH_catch(void);
#define NOPH_catch() while(0); do {  \
  asm volatile("");                  \
  __NOPH_catch();                    \
} while(0)



#endif /* __ASSEMBLER__ */

#endif /* !__CIBYL_H__ */
