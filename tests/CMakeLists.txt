cmake_minimum_required (VERSION 2.6)

# ====================================                                                                                        
# project name and version                                                                                                    
# ====================================                                                                                        
project (cibyl-tests)
enable_language(ASM)

# ====================================                                                                                        
# Setup the cross-compiler
# ====================================                                                                                        
include(../build/CMakeCibylToolchain.cmake)
set(CMAKE_TOOLCHAIN_FILE ../build/CMakeCibylToolchain.cmake)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "tmpclasses;res;include;classes")

execute_process(
	COMMAND cibyl-config --sysroot
	OUTPUT_VARIABLE SYSROOT
)
string(STRIP ${SYSROOT} SYSROOT)


add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/syscalls/command_mgr/include/command_mgr.h
	COMMAND cibyl-generate-c-header -o ${CMAKE_CURRENT_BINARY_DIR}/include/ ${CMAKE_CURRENT_SOURCE_DIR}/syscalls/ command_mgr
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmpclasses/Cibyl.j
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tests
	COMMAND cibyl-mips2java -O -DJSR075 --single-class -I${CMAKE_CURRENT_BINARY_DIR}/include -d ${CMAKE_CURRENT_BINARY_DIR}/tmpclasses/ ${CMAKE_CURRENT_BINARY_DIR}/tests
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tmpclasses/CRunTime.class
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tmpclasses/Cibyl.j
	COMMAND cp ${SYSROOT}/usr/share/java/Main.java ${CMAKE_CURRENT_BINARY_DIR}/tmpclasses
	COMMAND cp ${SYSROOT}/usr/share/java/GameScreenCanvas.java ${CMAKE_CURRENT_BINARY_DIR}/tmpclasses
	COMMAND cd tmpclasses && javac -classpath :${WTK_PATH}/lib/jsr75.jar -source 1.4 -bootclasspath ${WTK_PATH}/lib/cldcapi11.jar:${WTK_PATH}/lib/midpapi20.jar *.java
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CibylTest.jar
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tmpclasses/CRunTime.class
	COMMAND preverify -classpath ${WTK_PATH}/lib/cldcapi11.jar:${WTK_PATH}/lib/midpapi20.jar:${WTK_PATH}/lib/jsr75.jar -d classes/ tmpclasses/
	COMMAND install -d ${CMAKE_CURRENT_BINARY_DIR}/res
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/resources/file-which-exists ${CMAKE_CURRENT_BINARY_DIR}/res/
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/resources/b ${CMAKE_CURRENT_BINARY_DIR}/res/
	COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/tmpclasses/program.data.bin ${CMAKE_CURRENT_BINARY_DIR}/res/
	COMMAND jar cfm ${CMAKE_CURRENT_BINARY_DIR}/CibylTest.jar ${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.MF -C ${CMAKE_CURRENT_BINARY_DIR}/classes . -C ${CMAKE_CURRENT_BINARY_DIR}/res .
)

set(CMAKE_BUILD_TYPE distribution)
set(CMAKE_C_FLAGS_DISTRIBUTION "-Os")
set(CMAKE_EXE_LINKER_FLAGS "-static")
# CMake likes to include -rdynamic...
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") 
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "") 

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/c)
include_directories (${CMAKE_CURRENT_BINARY_DIR}/include)

set (tests_SRCS
	c/console.c
	c/main.c
	c/run-tests.c
	c/tests/64-bit-return.c
	c/tests/lui.c
	c/tests/and.c
	c/tests/malloc.c
	c/tests/char.c
	c/tests/memory.c
	c/tests/double.c
	c/tests/file-operations.c
	c/tests/pass-structs-by-value.c
	c/tests/float.c
	c/tests/relocation.c
	c/tests/function.c
	c/tests/shifts.c
	c/tests/int.c
	c/tests/slt.c
	c/tests/j2me.c
	c/tests/store-s-regs-c-helper.c
	c/tests/java-exceptions.c
	c/tests/unsigned-int.c
	c/tests/jr_test.c
	c/tests/varargs.c
	c/tests/libm.c
	c/tests/vmips_tests.c
	c/tests/vmips/bltz.S
	c/tests/vmips/load.S
	c/tests/vmips/multu.S
	c/tests/vmips/shifts.S
	c/tests/vmips/xor.S	
	c/tests/long-long.c
	c/tests/assign-to-ra.S
	c/tests/return_in_v1.S
	c/tests/tail_call.S
	c/tests/jr.S
	c/tests/store-s-regs.S
	c/tests/mul_tests.S
	c/tests/sw_in_delay_slot.S
	c/tests/astar/astar.c	
	c/tests/astar/finder.c
	c/tests/astar/prio_queue.c
	c/tests/astar/main.c
)

add_custom_target(syscalls ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include)

add_definitions(-DJSR075)
add_executable(tests ${tests_SRCS})
target_link_libraries(tests crt0 java m midp jsr075 java)
add_dependencies(tests syscalls)

add_custom_target(mips2java ALL DEPENDS tmpclasses/Cibyl.j)
add_custom_target(javac ALL DEPENDS tmpclasses/CRunTime.class)
add_custom_target(jar ALL DEPENDS CibylTest.jar)
