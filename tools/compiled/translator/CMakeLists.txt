set (translator_SRCS
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

include_directories (include/)

add_executable (xcibyl-translator ${translator_SRCS})
target_link_libraries (xcibyl-translator dw)
target_link_libraries (xcibyl-translator elf)
