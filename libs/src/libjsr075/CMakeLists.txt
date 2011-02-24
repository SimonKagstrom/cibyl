add_custom_target(libjsr075.a ALL
	COMMAND ${CMAKE_C_COMPILER} -Os -c -I ${CIBYL_BASE_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR}/fileconnection-filesystem.c -o ${LIBRARY_OUTPUT_PATH}/libjsr075.a)
