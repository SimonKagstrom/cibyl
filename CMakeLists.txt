cmake_minimum_required (VERSION 2.6)

# ====================================
# project name and version
# ====================================
project (cibyl)
set (PROJECT_VERSION_MAJOR  22)
set (PROJECT_VERSION_MINOR  0)
set (PROJECT_VERSION_PATCH  0)
set (PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

if(NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE  Debug)
endif(NOT CMAKE_BUILD_TYPE)

# Lookup where the compiler is located

execute_process(COMMAND which mips-cibyl-elf-gcc
  OUTPUT_VARIABLE MIPS_CIBYL_GCC_BINARY
  RESULT_VARIABLE which_RES)

if (which_RES)
  message("*** Can't find mips-linux-gcc, download the toolchain from http://cibyl.org
*** or build it in the toolchain/ directory\n")
  return(1)
endif (which_RES)

execute_process(COMMAND readlink -f ${MIPS_CIBYL_GCC_BINARY}
  OUTPUT_VARIABLE MIPS_CIBYL_GCC_READLINK)
execute_process(COMMAND dirname ${MIPS_CIBYL_GCC_READLINK}
  OUTPUT_VARIABLE MIPS_CIBYL_GCC_PATH)
string(STRIP ${MIPS_CIBYL_GCC_PATH} MIPS_CIBYL_GCC_PATH)


set (TOOLCHAIN_PATH ${MIPS_CIBYL_GCC_PATH}/../)
set (CIBYL_BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/mips-cibyl-elf/sys-root/lib)
set (LIBGCC_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/gcc/mips-cibyl-elf/4.4.5/)
set (INCLUDE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/mips-cibyl-elf/sys-root/usr/include)

set(DESTDIR ${TOOLCHAIN_PATH})
set(ENV{DESTDIR} ${TOOLCHAIN_PATH})


# ====================================
# build all
# ====================================
add_subdirectory (tools/translator)

# Otimize the target build
set(CMAKE_BUILD_TYPE distribution)
set(CMAKE_CXX_FLAGS_DISTRIBUTION "-Os -Wa,--no-warn -fno-optimize-sibling-calls")
set(CMAKE_C_FLAGS_DISTRIBUTION "-Os -Wa,--no-warn -fno-optimize-sibling-calls")

add_subdirectory (include)
add_subdirectory (tools)
add_subdirectory (syscalls)
add_subdirectory (libs)
add_subdirectory (java)


# ====================================
# installation
# ====================================
install (FILES
  ${CIBYL_BASE_PATH}/build/linker.lds
  DESTINATION ${LIBRARY_OUTPUT_PATH})
install (FILES
  ${CIBYL_BASE_PATH}/build/CMakeCibylToolchain.cmake
  DESTINATION ${LIBRARY_OUTPUT_PATH})
install (DIRECTORY
  ${CMAKE_BINARY_DIR}/bin
  USE_SOURCE_PERMISSIONS
  DESTINATION ${TOOLCHAIN_PATH})
install (DIRECTORY
  ${CMAKE_BINARY_DIR}/mips-cibyl-elf
  USE_SOURCE_PERMISSIONS
  DESTINATION ${TOOLCHAIN_PATH})
install (DIRECTORY
  ${CMAKE_BINARY_DIR}/lib
  USE_SOURCE_PERMISSIONS
  DESTINATION ${TOOLCHAIN_PATH})
