/*  ../vmips -o haltdumpcpu -o haltbreak mthi_lo.rom */
/*  should end with:  R09=7ffffffe R10=80000001  R12=ffffffff  R13=80000001 */
/*  tests instructions: mult multu */

	.text
	.globl  vmips_multu
        .ent    vmips_multu
vmips_multu:
	li $10, 0xffffffff
	li $13, 0x7fffffff
	li $14, 0xffffffff
	multu $10, $13
	mfhi $11
	mflo $12
	multu $10, $14
	mfhi $14
	mflo $15

        sw   $11, 0($4)
        sw   $12, 0($5)
        sw   $13, 0($6)
        sw   $14, 0($7)
        jr   $31

        .end    vmips_multu
