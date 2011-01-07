set (lib_SRCS
  64-bit-ops.c
  cache.c
  soft-float.c
  )

set (LIBRARY_OUTPUT_PATH ${LIBGCC_OUTPUT_PATH})
add_library (gcc ${lib_SRCS})
