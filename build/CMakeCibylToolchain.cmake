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




execute_process(COMMAND which preverify
  OUTPUT_VARIABLE WTK_PREVERIFY_PATH
  RESULT_VARIABLE which_RES)

if (which_RES)
  message("*** WARNING: Can't find Sun preverify command (for J2ME)\n"
"*** Add it to your PATH, or Cibyl won't be able to build J2ME programs.")
  set (WTK_PATH /)
else (which_RES)
  execute_process(COMMAND readlink -f ${WTK_PREVERIFY_PATH}
    OUTPUT_VARIABLE WTK_PREVERIFY_READLINK)
  execute_process(COMMAND dirname ${WTK_PREVERIFY_READLINK}
    OUTPUT_VARIABLE WTK_PREVERIFY_PATH)
  string(STRIP ${WTK_PREVERIFY_PATH} WTK_PREVERIFY_PATH)
  set (WTK_PATH ${WTK_PREVERIFY_PATH}/../)
endif (which_RES)

