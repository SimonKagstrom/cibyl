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

	addiu	$29, $5, -8	# sp = size of memory - 8

	sub	$5, $5, $3	# heap end (minus stack)
	jal	smalloc_set_memory_pool	# init malloc

	## Run global constructors
	jal	crt0_run_global_constructors

        jr      $31             # Back to the caller
.end	__start

CIBYL_EXPORT_SYMBOL(__start)


.section .data0
.globl cibyl_memdebug_start_addr	# Export to C
.globl cibyl_memdebug_end_addr
metainfo:
	.word	__start			# Start address
	.word	NOPH_STACK_SIZE		# Stack size
	.word	__edata			# End of allocated data
	.word	0			# Allocated memory (set by CRunTime.java)
cibyl_memdebug_start_addr:
	.word	0			# Start address to check if --memory-debug is on
cibyl_memdebug_end_addr:
	.word	4			# End address to check if --memory-debug is on

### Force something into .rodata
.section .rodata
	.long	0
