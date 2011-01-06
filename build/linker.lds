OUTPUT_ARCH(mips)
STARTUP(crt0.o)
GROUP(-lcrt0 -lc -ljava)
ENTRY(__start)

SECTIONS
{
       	. = 0x1000000;
       	.text : ALIGN(4) { *(.init) *(.text) *(.text.*) *(.gnu.linkonce.t.*) }
       	. = 0x0;
       	.data : ALIGN(16) { *(.data0) *(.data) *(.rodata.str1.4)  *(.rodata.cst4) *(.rodata.cst8) *(.reginfo) }
       	.rodata : ALIGN(16) { *(.rodata) *(.rodata.*) *(.gnu.linkonce.r.*) *(.cibylsetjmp) }

	.ctors : ALIGN(4) {
	       PROVIDE(__ctors_begin = .);
	       *(.ctors)
	       PROVIDE(__ctors_end = .);
	}
	.dtors : ALIGN(4) {
	       PROVIDE(__dtors_begin = .);
	       *(.dtors)
	       PROVIDE(__dtors_end = .);
	}
       	.bss : ALIGN(16) {
	     *(.bss) *(.scommon) *(.sbss)
	     PROVIDE(__edata = .);
	}

        /* Exported symbols (to Java) */
       	.cibylexpsyms : { *(.cibylexpsyms) }

	/* The string table with the exception names */
	. = 0xfd000000;
       	.cibylexceptionstrs : { *(.cibylexceptionstrs) }

	/* The string table with the system call names */
	. = 0xff000000;
       	.cibylstrtab : { *(.cibylstrtab) }
}
