/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      cibyl-debug.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Exported interface for the stack dumper
 *
 * $Id: cibyl-debug.h 12135 2006-11-16 06:38:38Z ska $
 *
 ********************************************************************/
#ifndef __CIBYL_DEBUG_H__
#define __CIBYL_DEBUG_H__

extern unsigned long cibyl_debug_regs;
extern void cibyl_dump_stack_helper(void);

#define cibyl_dump_stack()                           \
  do {                                               \
    unsigned long reg_saver;                         \
    asm volatile("sw    $25, %[reg_saver]\n"         \
		 "la    $25, %[reg_address]\n"       \
		 "sw	$1,  1*4($25)\n"             \
		 "sw	$2,  2*4($25)\n"             \
		 "sw	$3,  3*4($25)\n"	     \
		 "sw	$4,  4*4($25)\n"	     \
		 "sw	$5,  5*4($25)\n"	     \
		 "sw	$6,  6*4($25)\n"	     \
		 "sw	$7,  7*4($25)\n"	     \
		 "sw	$8,  8*4($25)\n"             \
		 "sw	$9,  9*4($25)\n"	     \
		 "sw	$10, 10*4($25)\n"	     \
		 "sw	$11, 11*4($25)\n"	     \
		 "sw	$12, 12*4($25)\n"	     \
		 "sw	$13, 13*4($25)\n"	     \
		 "sw	$14, 14*4($25)\n"	     \
		 "sw	$15, 15*4($25)\n"	     \
		 "sw	$16, 16*4($25)\n"	     \
		 "sw	$17, 17*4($25)\n"	     \
		 "sw	$18, 18*4($25)\n"	     \
		 "sw	$19, 19*4($25)\n"	     \
		 "sw	$20, 20*4($25)\n"            \
		 "sw	$21, 21*4($25)\n"	     \
		 "sw	$22, 22*4($25)\n"	     \
		 "sw	$23, 23*4($25)\n"	     \
		 "sw	$24, 24*4($25)\n"	     \
		 "sw	$25, 25*4($25)\n"	     \
		/* "sw	$26, 26*4($25)\n" k0 */	     \
		/* "sw	$27, 27*4($25)\n" k1 */	     \
		 "sw	$28, 28*4($25)\n"	     \
		 "sw	$29, 29*4($25)\n"	     \
		 "sw	$30, 30*4($25)\n"	     \
		 "sw	$31, 31*4($25)\n"	     \
                 "lw    $25, %[reg_saver]\n"         \
		 :                                   \
		 : [reg_saver]"m"(reg_saver), [reg_address]"i"(&cibyl_debug_regs) \
		 );                                  \
     cibyl_dump_stack_helper();                      \
  } while (0)

#endif /* !__CIBYL_DEBUG_H__ */
