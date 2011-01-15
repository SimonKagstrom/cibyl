INCLUDE(../build/CMakeCibylToolchain.cmake)
SET(CMAKE_TOOLCHAIN_FILE ../build/CMakeCibylToolchain.cmake)

include_directories (${INCLUDE_OUTPUT_PATH})
include_directories (${CIBYL_BASE_PATH}/include) # Not needed after install

add_subdirectory(src/libc)
add_subdirectory(src/libcibar)
add_subdirectory(src/libcrt0)
add_subdirectory(src/libgcc)
add_subdirectory(src/libj2se)
add_subdirectory(src/libjava)
add_subdirectory(src/libjsr075)
add_subdirectory(src/libm)
add_subdirectory(src/libmidp)
add_subdirectory(src/libs9)
add_subdirectory(src/libstdc++)

add_custom_target (low-level-gcc ALL
  COMMAND ${CMAKE_C_COMPILER} -c -I ${CIBYL_BASE_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR}/crt0.S -o ${LIBRARY_OUTPUT_PATH}/crt0.o
  COMMAND install -d ${LIBGCC_OUTPUT_PATH}
  COMMAND ${CMAKE_C_COMPILER} -c -I ${CIBYL_BASE_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR}/crtbegin.S -o ${LIBGCC_OUTPUT_PATH}/crtbegin.o
  COMMAND ${CMAKE_C_COMPILER} -c -I ${CIBYL_BASE_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR}/crtend.S -o ${LIBGCC_OUTPUT_PATH}/crtend.o
)
