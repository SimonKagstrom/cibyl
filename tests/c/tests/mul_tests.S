/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      mul_tests.S
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Tests of the translator
 *
 * $Id:$
 *
 ********************************************************************/
.globl mul_test_no_hilo
.ent mul_test_no_hilo
mul_test_no_hilo:
        mult    $3, $4
        jr      $31
        nop
.end mul_test_no_hilo
