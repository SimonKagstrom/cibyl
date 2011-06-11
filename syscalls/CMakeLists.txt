# Add a command to generate Cibyl syscall databases
add_custom_command (
  OUTPUT ${INCLUDE_OUTPUT_PATH}/cibyl-syscalls.db
  COMMAND ${CIBYL_BASE_PATH}/tools/cibyl-generate-c-header -o ${INCLUDE_OUTPUT_PATH} ${CMAKE_CURRENT_SOURCE_DIR} softfloat resource_manager jmicropolygon android
  )

ADD_CUSTOM_TARGET(cibyl-syscalls ALL DEPENDS ${INCLUDE_OUTPUT_PATH}/cibyl-syscalls.db)

install (DIRECTORY
  ${CIBYL_BASE_PATH}/syscalls
  DESTINATION ${TOOLCHAIN_PATH}/mips-cibyl-elf/sys-root/usr/share)
