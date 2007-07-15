/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      jr.S
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   local jump test
 *
 * $Id:$
 *
 ********************************************************************/
.data
lbl:    .long   fwd
.text
.globl jr_test_asm
.ent jr_test_asm
jr_test_asm:
	lw	$4, lbl		# Cibyl is not too happy about local labels
	li	$2, 0
	jr	$4
	li	$2, 1		# Should never happen
fwd:
	jr	$31		# jr ra
        j       fwd
.end jr_test_asm
