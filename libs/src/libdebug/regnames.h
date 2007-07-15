/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      regnames.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   MIPS register names
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __REGNAMES_H__
#define __REGNAMES_H__

#define N_REGS 32

#if defined(__ASSEMBLER__)
#define zero $0
#define AT   $at
#define v0   $2
#define v1   $3
#define a0   $4
#define a1   $5
#define a2   $6
#define a3   $7
#define t0   $8
#define t1   $9
#define t2   $10
#define t3   $11
#define t4   $12
#define t5   $13
#define t6   $14
#define t7   $15
#define s0   $16
#define s1   $17
#define s2   $18
#define s3   $19
#define s4   $20
#define s5   $21
#define s6   $22
#define s7   $23
#define t8   $24
#define t9   $25
#define k0   $26
#define k1   $27
#define gp   $28
#define sp   $29
#define s8   $30
#define fp   $30
#define ra   $31
#endif

#define	R_R0	0
#define	R_R1	1
#define	R_R2	2
#define	R_R3	3
#define	R_R4	4
#define	R_R5	5
#define	R_R6	6
#define	R_R7	7
#define	R_R8	8
#define	R_R9	9
#define	R_R10	10
#define	R_R11	11
#define	R_R12	12
#define	R_R13	13
#define	R_R14	14
#define	R_R15	15
#define	R_R16	16
#define	R_R17	17
#define	R_R18	18
#define	R_R19	19
#define	R_R20	20
#define	R_R21	21
#define	R_R22	22
#define	R_R23	23
#define	R_R24	24
#define	R_R25	25
#define	R_R26	26
#define	R_R27	27
#define	R_R28	28
#define	R_R29	29
#define	R_R30	30
#define	R_R31	31

#define	R_ZERO	R_R0
#define	R_AT	R_R1
#define	R_V0	R_R2
#define	R_V1	R_R3
#define	R_A0	R_R4
#define	R_A1	R_R5
#define	R_A2	R_R6
#define	R_A3	R_R7
#define	R_T0	R_R8
#define	R_T1	R_R9
#define	R_T2	R_R10
#define	R_T3	R_R11
#define	R_T4	R_R12
#define	R_T5	R_R13
#define	R_T6	R_R14
#define	R_T7	R_R15
#define	R_S0	R_R16
#define	R_S1	R_R17
#define	R_S2	R_R18
#define	R_S3	R_R19
#define	R_S4	R_R20
#define	R_S5	R_R21
#define	R_S6	R_R22
#define	R_S7	R_R23
#define	R_T8	R_R24
#define	R_T9	R_R25
#define	R_K0	R_R26
#define	R_K1	R_R27
#define	R_GP	R_R28
#define	R_SP	R_R29
#define	R_FP	R_R30
#define	R_RA	R_R31

#endif /* !__REGNAMES_H__ */
