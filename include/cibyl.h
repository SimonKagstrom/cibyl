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

# define NOPH_SECTION(x) __attribute__((section (x)))

/* Convenience */
typedef int bool_t;

/* Special object! */
typedef int NOPH_Exception_t;

/* Include the generated syscall definitions
 *
 * Thanks to Ian Lance Taylor for the use of .set push / .set pop.
 */
# include "cibyl-syscall_defs.h"

extern void __NOPH_try(void (*callback)(NOPH_Exception_t exception, void *arg), void *arg);

/**
 * Begin a block of catching Java exceptions. Java exceptions will be
 * catched within the block after NOPH_try (ended by NOPH_catch). The
 * exception handling works as in Java, so the handler will return to
 * the statement after the NOPH_catch() statement. The block must be
 * closed by a @a NOPH_catch.
 *
 * @param callback the callback to call if an exception occurs in the block
 * @param arg the argument to pass to the callback
 */
#define NOPH_try(callback, arg) do { \
  asm volatile("");                  \
  __NOPH_try(callback, arg);         \
} while(0); do

/**
 * End a Java exception-catching block, catching all exceptions. Must
 * be preceeded by a @a NOPH_try
 *
 * @see NOPH_try
 */
extern void __NOPH_catch(void);
#define NOPH_catch() while(0); do {  \
  asm volatile("");                  \
  __NOPH_catch();                    \
} while(0)

extern void __NOPH_throw(NOPH_Exception_t exception);
#define NOPH_throw(exception) do { \
  asm volatile("");                \
  __NOPH_throw(exception);         \
} while(0)

/**
 * End a Java exception-catching block, catching a specific class of
 * exceptions. Must be preceeded by a @a NOPH_try.
 *
 * @warning This is not yet implemented and will now to the same as @a
 * NOPH_catch
 *
 * @see NOPH_try, NOPH_catch
 */
#define NOPH_catch_exception(exceptionClass) NOPH_catch()

/**
 * Standard exception handler that sets the argument to 1 if an
 * exception occurred. This should be passed to @a NOPH_try, and the
 * argument is a typecasted int pointer. Example:
 *
 * <pre>
 * int error = 0;
 *
 * NOPH_try(NOPH_setter_exception_handler, &error) {
 * ...
 * } NOPH_catch()
 * if (error)
 *    printf("An exception occurred - do something here!");
 * </pre>
 *
 * @param the exception (passed automatically)
 * @param the exception argument (passed automatically). This is typecast
 *        to an int pointer
 *
 * @see NOPH_try, NOPH_catch
 *
 */
void NOPH_setter_exception_handler(NOPH_Exception_t ex, void *arg);

/**
 * Crash the system on unrecoverable errors.
 *
 * @param fmt format string to print if the error happens
 * @parma ... printf-style arguments to @a fmt
 */
void NOPH_panic(const char *fmt, ...);

#define NOPH_panic_if(cond, fmt...) do { \
  if (cond)                              \
    NOPH_panic(fmt);                     \
} while(0)

#endif /* __ASSEMBLER__ */

#endif /* !__CIBYL_H__ */
