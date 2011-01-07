set (lib_SRCS
  connector-filesystem.c
  recordstore-filesystem.c
  resource-filesystem.c
  )

add_library (midp ${lib_SRCS})
