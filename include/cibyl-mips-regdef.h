/*
 * include/asm-mips/regdefs.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 1995 by Ralf Baechle
 */

#ifndef __ASM_MIPS_REGDEF_H
#define __ASM_MIPS_REGDEF_H

/*
 * Symbolic register names for 32 bit ABI
 */
#define zero    $0      /* wired zero */
#define AT      $1      /* assembler temp  - uppercase because of ".set at" */
#define v0      $2      /* return value */
#define v1      $3
#define a0      $4      /* argument registers */
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8      /* caller saved */
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16     /* callee saved */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24     /* caller saved */
#define t9      $25
#define jp      $25     /* PIC jump register */
#define k0      $26     /* kernel scratch */
#define k1      $27
#define gp      $28     /* global pointer */
#define sp      $29     /* stack pointer */
#define fp      $30     /* frame pointer */
#define s8	$30	/* same like fp! */
#define ra      $31     /* return address */


/*
 * relative position of registers in save reg area
 */
#define	R_R0		0
#define	R_R1		1
#define	R_R2		2
#define	R_R3		3
#define	R_R4		4
#define	R_R5		5
#define	R_R6		6
#define	R_R7		7
#define	R_R8		8
#define	R_R9		9
#define	R_R10		10
#define	R_R11		11
#define	R_R12		12
#define	R_R13		13
#define	R_R14		14
#define	R_R15		15
#define	R_R16		16
#define	R_R17		17
#define	R_R18		18
#define	R_R19		19
#define	R_R20		20
#define	R_R21		21
#define	R_R22		22
#define	R_R23		23
#define	R_R24		24
#define	R_R25		25
#define	R_R26		26
#define	R_R27		27
#define	R_R28		28
#define	R_R29		29
#define	R_R30		30
#define	R_R31		31
#define	R_F0		32
#define	R_F1		33
#define	R_F2		34
#define	R_F3		35
#define	R_F4		36
#define	R_F5		37
#define	R_F6		38
#define	R_F7		39
#define	R_F8		40
#define	R_F9		41
#define	R_F10		42
#define	R_F11		43
#define	R_F12		44
#define	R_F13		45
#define	R_F14		46
#define	R_F15		47
#define	R_F16		48
#define	R_F17		49
#define	R_F18		50
#define	R_F19		51
#define	R_F20		52
#define	R_F21		53
#define	R_F22		54
#define	R_F23		55
#define	R_F24		56
#define	R_F25		57
#define	R_F26		58
#define	R_F27		59
#define	R_F28		60
#define	R_F29		61
#define	R_F30		62
#define	R_F31		63
#define NCLIENTREGS	64
#define	R_EPC		64
#define	R_MDHI		65
#define	R_MDLO		66
#define	R_SR		67
#define	R_CAUSE		68
#define	R_TLBHI		69
#define	R_TLBLO		70
#define	R_BADVADDR	71
#define	R_INX		72
#define	R_RAND		73
#define	R_CTXT		74
#define	R_EXCTYPE	75
#define R_MODE		76
#define	R_PRID		77
#define R_FCSR		78
#define R_FEIR		79
#define	NREGS		80

/*
 * For those who like to think in terms of the compiler names for the regs
 */
#define	R_ZERO		R_R0
#define	R_AT		R_R1
#define	R_V0		R_R2
#define	R_V1		R_R3
#define	R_A0		R_R4
#define	R_A1		R_R5
#define	R_A2		R_R6
#define	R_A3		R_R7
#define	R_T0		R_R8
#define	R_T1		R_R9
#define	R_T2		R_R10
#define	R_T3		R_R11
#define	R_T4		R_R12
#define	R_T5		R_R13
#define	R_T6		R_R14
#define	R_T7		R_R15
#define	R_S0		R_R16
#define	R_S1		R_R17
#define	R_S2		R_R18
#define	R_S3		R_R19
#define	R_S4		R_R20
#define	R_S5		R_R21
#define	R_S6		R_R22
#define	R_S7		R_R23
#define	R_T8		R_R24
#define	R_T9		R_R25
#define	R_K0		R_R26
#define	R_K1		R_R27
#define	R_GP		R_R28
#define	R_SP		R_R29
#define	R_FP		R_R30
#define	R_RA		R_R31


#define c0_status $12    /* status register */
#define c0_cause  $13    /* exception cause */
#define c0_epc    $14    /* exception PC */

#define C0_SR_BEV (1<<22) /* boot exception vector */

#if defined(__ASSEMBLER__)
/* Assembly macros to save and restore registers
 *
 * Argument: where to store the registers
 *
 * Clobbers k0
 */
.macro	save_regs where
	la	k0, \where
.set	noat
	sw	AT, R_AT*4(k0)
.set	at
	sw	v0, R_V0*4(k0)
	sw	v1, R_V1*4(k0)
	sw	a0, R_A0*4(k0)
	sw	a1, R_A1*4(k0)
	sw	a2, R_A2*4(k0)
	sw	a3, R_A3*4(k0)
	sw	t0, R_T0*4(k0)
	sw	t1, R_T1*4(k0)
	sw	t2, R_T2*4(k0)
	sw	t3, R_T3*4(k0)
	sw	t4, R_T4*4(k0)
	sw	t5, R_T5*4(k0)
	sw	t6, R_T6*4(k0)
	sw	t7, R_T7*4(k0)
	sw	s0, R_S0*4(k0)
	sw	s1, R_S1*4(k0)
	sw	s2, R_S2*4(k0)
	sw	s3, R_S3*4(k0)
	sw	s4, R_S4*4(k0)
	sw	s5, R_S5*4(k0)
	sw	s6, R_S6*4(k0)
	sw	s7, R_S7*4(k0)
	sw	t8, R_T8*4(k0)
	sw	t9, R_T9*4(k0)
	sw	gp, R_GP*4(k0)
	sw	sp, R_SP*4(k0)
	sw	fp, R_FP*4(k0)
	sw	ra, R_RA*4(k0)
.endm	save_regs


.macro	restore_regs where
	la	k0, \where
.set	noat
	lw	AT, R_AT*4(k0)
.set	at
	lw	v0, R_V0*4(k0)
	lw	v1, R_V1*4(k0)
	lw	a0, R_A0*4(k0)
	lw	a1, R_A1*4(k0)
	lw	a2, R_A2*4(k0)
	lw	a3, R_A3*4(k0)
	lw	t0, R_T0*4(k0)
	lw	t1, R_T1*4(k0)
	lw	t2, R_T2*4(k0)
	lw	t3, R_T3*4(k0)
	lw	t4, R_T4*4(k0)
	lw	t5, R_T5*4(k0)
	lw	t6, R_T6*4(k0)
	lw	t7, R_T7*4(k0)
	lw	s0, R_S0*4(k0)
	lw	s1, R_S1*4(k0)
	lw	s2, R_S2*4(k0)
	lw	s3, R_S3*4(k0)
	lw	s4, R_S4*4(k0)
	lw	s5, R_S5*4(k0)
	lw	s6, R_S6*4(k0)
	lw	s7, R_S7*4(k0)
	lw	t8, R_T8*4(k0)
	lw	t9, R_T9*4(k0)
	lw	gp, R_GP*4(k0)
	lw	sp, R_SP*4(k0)
	lw	ra, R_RA*4(k0)
.endm	restore_regs
#endif /* __ASSEMBLY__ */

#endif /* __ASM_MIPS_REGDEF_H */
