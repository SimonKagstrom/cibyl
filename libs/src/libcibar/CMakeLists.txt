
add_custom_target(libcibar.a ALL
	COMMAND ${CMAKE_C_COMPILER} -Os -c -I${CIBYL_BASE_PATH}/include -I${INCLUDE_OUTPUT_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cibar.c -o ${LIBRARY_OUTPUT_PATH}/libcibar.a)
