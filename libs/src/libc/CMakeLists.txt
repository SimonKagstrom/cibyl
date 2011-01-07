set (lib_SRCS
	ansi.c
	smalloc.c
	stdlib.c
	time.c
	scanf_ska.c
	printf.c
	vsnprintf.c
	string.c
	)

add_library (c ${lib_SRCS})
