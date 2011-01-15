#ifndef MIPS_DWARF_H
#define MIPS_DWARF_H

#include <dwarf.h>
#include <elfutils/libdw.h>


#if defined(__cplusplus)
extern "C" {
#endif

enum mips_arg
{
	UNKNOWN = -1,
	VOID = 0,
	N_1 = 1,
	N_2 = 2,
	N_3 = 3,
	N_4 = 4,
	AGGREGATE = 5,
};

enum mips_arg mips_arg_size (Elf *elf, Dwarf_Die *functypedie, Dwarf_Attribute *attr);


#if defined(__cplusplus)
}
#endif

#endif
