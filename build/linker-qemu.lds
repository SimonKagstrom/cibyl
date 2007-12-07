OUTPUT_ARCH(mips)
ENTRY(__qemu_start)

SECTIONS
{
       	. = 0x80000180;
       	.text ALIGN(4)  : { *(.init.exception) }
       	. = 0x80010000;
       	.text ALIGN(4)  : { *(.init) *(.text) *(.text.*) }

	/* Data directly after the text */
       	.data ALIGN(16) : { *(.data0) *(.data) *(.rodata.str1.4)  *(.rodata.cst4) *(.rodata.cst8) *(.reginfo) }
       	.rodata ALIGN(16) : { *(.rodata) *(.rodata.*) }
	PROVIDE(__ctors_begin = .);
	.ctors ALIGN(4): { *(.ctors) }
	PROVIDE(__ctors_end = .);
	PROVIDE(__dtors_begin = .);
	.dtors ALIGN(4): { *(.dtors) }
	PROVIDE(__dtors_end = .);
       	.bss  ALIGN(16) : { *(.bss) *(.scommon) *(.sbss) }
       	PROVIDE(__edata = .);

	/* The string table with the system call names */
	. = 0xff000000;
       	.cibylstrtab : { *(.cibylstrtab) }
}
