/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      cibyl-softfloat.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl softfloat support
 *
 * $Id: cibyl-softfloat.h 14109 2007-03-10 08:19:13Z ska $
 *
 ********************************************************************/
#ifndef __CIBYL_SOFTFLOAT_H__
#define __CIBYL_SOFTFLOAT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>
#include <stdint.h>

union s_float_union { float f; uint32_t i; };
union s_double_union { double f; uint64_t i; };

typedef union s_float_union float_union_t;
typedef union s_double_union double_union_t;

int __addsf3_helper(int _a, int _b); /* Not generated */
int __subsf3_helper(int _a, int _b); /* Not generated */
int __mulsf3_helper(int _a, int _b); /* Not generated */
int __divsf3_helper(int _a, int _b); /* Not generated */
int __negsf2_helper(int _a); /* Not generated */
int __isnanf_helper(int _a); /* Not generated */
int __isinff_helper(int _a); /* Not generated */
int __fixunssfsi_helper(int _a); /* Not generated */

void __adddf3_helper(void* address_res, void* address_A, void* address_B); /* Not generated */
void __muldf3_helper(void* address_res, void* address_A, void* address_B); /* Not generated */
void __divdf3_helper(void* address_res, void* address_A, void* address_B); /* Not generated */
void __subdf3_helper(void* address_res, void* address_A, void* address_B); /* Not generated */
void __negdf2_helper(void* address_res, void* address_A); /* Not generated */
int __isnan_helper(void* address_A); /* Not generated */
int __isinf_helper(void* address_A); /* Not generated */
int __fixunsdfsi_helper(void* address_A); /* Not generated */

int __fixsfsi_helper(int _a); /* Not generated */
int __fixdfsi_helper(void* address); /* Not generated */
int __floatsisf_helper(int i); /* Not generated */
int __cmpsf2_helper(int _a, int _b); /* Not generated */

int __floatunsisf_helper(int i); /* Not generated */
void __floatunsidf_helper(void* address, int i); /* Not generated */
void __floatsidf_helper(void* address, int i); /* Not generated */
int __cmpdf2_helper(void* address_a, void* address_b); /* Not generated */

void __extendsfdf2_helper(void* address, int a); /* Not generated */
int __truncdfsf2_helper(void* address); /* Not generated */

/* math.h stuff */
int __sinf_helper(int _a); /* Not generated */
int __cosf_helper(int _a); /* Not generated */
int __atanf_helper(int _a); /* Not generated */
int __asinf_helper(int _a); /* Not generated */
int __acosf_helper(int _a); /* Not generated */
int __atan2f_helper(int _a, int _b); /* Not generated */
int __powf_helper(int _a, int _b); /* Not generated */
int __log10f_helper(int _a); /* Not generated */
int __logf_helper(int _a); /* Not generated */
int __expf_helper(int _a); /* Not generated */
int __floorf_helper(int _a); /* Not generated */
int __sqrtf_helper(int _a); /* Not generated */

void __sin_helper(void* address_res, void* address_a); /* Not generated */
void __cos_helper(void* address_res, void* address_a); /* Not generated */
void __atan_helper(void* address_res, void* address_a); /* Not generated */
void __asin_helper(void* address_res, void* address_a); /* Not generated */
void __acos_helper(void* address_res, void* address_a); /* Not generated */
void __log10_helper(void* address_res, void* address_a); /* Not generated */
void __log_helper(void* address_res, void* address_a); /* Not generated */
void __atan2_helper(void* address_res, void* address_a, void* address_b); /* Not generated */
void __pow_helper(void* address_res, void* address_a, void* address_b); /* Not generated */
void __exp_helper(void* address_res, void* address_a); /* Not generated */
void __floor_helper(void* address_res, void* address_a); /* Not generated */
void __sqrt_helper(void* address_res, void* address_a); /* Not generated */

#if defined(__cplusplus)
}
#endif
#endif /* !__CIBYL_SOFTFLOAT_H__ */
