/*  ../vmips -o haltdumpcpu -o haltbreak load.rom */
/*  should end with: R09=deadbeef R10=0000beef  R11=000000ef */
/*  tests instructions: lw lh lb */


	.text
	.globl vmips_load
        .ent vmips_load
vmips_load:
	la	$10, loser
	lw	$11, 0($10)
	la	$10, hoser
	lh	$12, 0($10)
	la	$10, booger
	lb	$13, 0($10)

        sw      $11, 0($4)
        sw      $12, 0($5)
        sw      $13, 0($6)
        jr      $31
.end vmips_load

.data
.align  2
loser:	.word	0xDEADBEEF
hoser:	.word	0x1234BEEF
booger:	.word	0x123456EF
