#include <cibyl.h>

.section .init
.globl  __start
.ent	__start

	## Entry point:
	## initialize the C runtime and call main.
__start:
.set push
	## Initialize the heap
	la	$2, metainfo
	lw	$3, 4($2)	# stack size
	lw	$4, 8($2)	# start of memory -> a0 (start of .bss)
	lw	$5, 12($2)	# size of memory -> a1

	sub	$29, $5, $3	# Setup stack pointer to
	addiu	$29, $29, -8	# size of memory - stack_size - 8

	sub	$5, $5, $3	# heap end (minus stack)
	jal	smalloc_set_memory_pool	# init malloc

	## Run global constructors
	jal	crt0_run_global_constructors

	## Start the program
        move    $4, $0
        move    $5, $0
	jal	main
	nop

        ## global destructors should be run by exit()
	move	$4, $2		# move a0, v0
	jal	exit
	nop
.end	__start

.section .data0
metainfo:
	.word	__start			# Start address
	.word	NOPH_STACK_SIZE		# Stack size
	.word	__edata			# End of allocated data
	.word	0			# Allocated memory (set by CRunTime.java)

### Force something into .rodata
.section .rodata
	.long	0
