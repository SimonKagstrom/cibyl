# this one is important
set(CMAKE_SYSTEM_NAME Cibyl)
#this one not so much
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
set(CMAKE_ASM_COMPILER  mips-cibyl-elf-gcc)
set(CMAKE_C_COMPILER   mips-cibyl-elf-gcc)
set(CMAKE_CXX_COMPILER mips-cibyl-elf-g++)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_ASM_FLAGS "-c")
