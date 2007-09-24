/*  ../vmips -o haltdumpcpu -o haltbreak bltz.rom */
/*  should end with: R10=00000001 R11=00000001 R12=00000000 */
/*  tests instructions: bltz */

	.text
	.globl vmips_bltz
        .ent vmips_bltz
vmips_bltz:
       	li $10, 0
	li $11, 0
	li $12, 0
	li $13, 1
	bltz $13, 1f
	addiu $10, $10, 1
1:	li $13, 0
	bltz $13, 2f
	addiu $11, $11, 1
2:	li $13, -1
	bltz $13, 3f
	addiu $12, $12, 1
3:
        sw      $10, 0($4)
        sw      $11, 0($5)
        sw      $12, 0($6)
        jr      $31

.end vmips_bltz
