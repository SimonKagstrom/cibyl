add_custom_target(libmidp.a ALL
	COMMAND ${CMAKE_C_COMPILER} -Os -c -I ${CIBYL_BASE_PATH}/include -I${INCLUDE_OUTPUT_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/connector-filesystem.c -o ${LIBRARY_OUTPUT_PATH}/libmidp.a)
