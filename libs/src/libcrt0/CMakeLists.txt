set (lib_SRCS
	fileops.c
	helpers.c
	memory-filesystem.c)

add_library (crt0 ${lib_SRCS})
