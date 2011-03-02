add_custom_target(libj2se.a ALL
	COMMAND ${CMAKE_C_COMPILER} -Os -c -I ${CIBYL_BASE_PATH}/include -I${INCLUDE_OUTPUT_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/file.c -o ${LIBRARY_OUTPUT_PATH}/libj2se.a)
