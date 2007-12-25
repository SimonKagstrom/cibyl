#include <cibyl.h>
#include <cibyl-mips-regdef.h>

.section .bss
scratch_area:	.fill	32, 4, 0
qemu_stk:	.fill	128, 4, 0

.section .data
handlers:
	.long	__qemu_default    #  0 interrupt
	.long	__qemu_default    #  4 TLB modification
	.long	__qemu_default    #  8 TLB exception (load)
	.long	__qemu_default    #  c TLB exception (store)
	.long	__qemu_default    # 10 Address exception (load)
	.long	__qemu_default    # 14 Address exception (store)
	.long	__qemu_default    # 18 Bus error (insn)
	.long	__qemu_default    # 1c Bus error (data)
	.long	__qemu_default    # 20 Syscall
	.long	__qemu_default    # 24 Breakpoint
	.long	__qemu_reserved   # 28 Reserved instruction
	.long	__qemu_default    # 2c Coprocessor unusable
	.long	__qemu_default    # 30 Arithmetic overflow
	.long	__qemu_default    # 34 Trap exception
	.long	__qemu_default    # 38 -
	.long	__qemu_default    #    FPU exception
	.long	__qemu_default    #    -
	.long	__qemu_default    #    CP2 exception
	.long	__qemu_default    #    -
	.long	__qemu_default    #    MDMX exception
	.long	__qemu_default    #    WatchHi/WatchLo
	.long	__qemu_default    #    Machine check
	.long	__qemu_default    #    -
	.long	__qemu_default    #    Cache error
	.long	__qemu_default    #    -

.section .init.exception
## located at 0x80000180
.ent	exc_handler
exc_handler:
	save_regs scratch_area

	la	sp, qemu_stk + 128 * 4 - 8

	la	v0, handlers
	la	a0, scratch_area
	mfc0	a1, c0_status
	mfc0	a2, c0_cause
	mfc0	a3, c0_epc

	andi	t0, a2, 0x3c	# mask out ExcCode
	addu	v0, v0, t0	# address of handler address
	lw	v0, 0(v0)	# load it

	jalr	v0
	nop
	move	k1, v0
	li	k0, 4
	mtc0	k0, c0_status
	restore_regs scratch_area
.set	noreorder
	jr	k1
	nop
.set	reorder
.end	exc_handler

.section .init
	## Entry point:
	## initialize the C runtime and call main.
.globl  __qemu_start
.ent	__qemu_start
__qemu_start:
.set push
	## Enable exceptions to the installed handler
	mfc0	t0, c0_status
	li	t1, ~C0_SR_BEV # Set the exception vector to 0x80000080
	and	t0, t0, t1 # Set the exception vector to 0x80000080
	mtc0	t0, c0_status


	la	a0, __cibylstrtab_begin
	la	a1, __cibylstrtab_end
	.long	0xfefe0004	# pass a0
	.long	0xfefe0005	# pass a1
	.long	0xffffffff	# special case - pass the string table!

	j	__start
	nop
.end __qemu_start
