set (translator_SRCS
	external/elfutils/dwarf_abbrev_hash.c
	external/elfutils/dwarf_attr.c
	external/elfutils/dwarf_attr_integrate.c
	external/elfutils/dwarf_begin_elf.c
	external/elfutils/dwarf_child.c
	external/elfutils/dwarf_end.c
	external/elfutils/dwarf_end.c
	external/elfutils/dwarf_error.c
	external/elfutils/dwarf_formaddr.c
	external/elfutils/dwarf_formref.c
	external/elfutils/dwarf_formref_die.c
	external/elfutils/dwarf_formudata.c
	external/elfutils/dwarf_getabbrev.c
	external/elfutils/dwarf_hasattr.c
	external/elfutils/dwarf_hasattr_integrate.c
	external/elfutils/dwarf_lowpc.c
	external/elfutils/dwarf_nextcu.c
	external/elfutils/dwarf_offdie.c
	external/elfutils/dwarf_siblingof.c
	external/elfutils/dwarf_sig8_hash.c
	external/elfutils/dwarf_tag.c
	external/elfutils/frame-cache.c
	external/elfutils/libdw_alloc.c
	external/elfutils/libdw_findcu.c
	external/elfutils/libdw_form.c
	basicblock.cc
    builtins.cc
    calltablemethod.cc
    codeblock.cc
	controller.cc
    elf.cc
    emit.cc
    function.cc
    functioncolocation.cc
	instruction.cc
	javamethod.cc
    javaclass.cc
    mips.cc
    mips-dwarf.c
    registerallocator.cc
    string-instruction.cc
    syscall-wrappers.cc
    utils.cc)

include_directories(
	external/elfutils/
	external/
	include/
)

add_executable (xcibyl-translator ${translator_SRCS})
if (CYGWIN)
	target_link_libraries (xcibyl-translator elf intl)
else()
	target_link_libraries (xcibyl-translator elf)
endif()
