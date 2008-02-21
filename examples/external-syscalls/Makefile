######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 13560 2007-02-10 07:39:53Z ska $
##
######################################################################
SUBDIRS = c
RESOURCES=res/program.data.bin
SOURCES = src/CRunTime.java src/StandaloneMain.java src/Syscalls.java src/CibylConfig.java
TARGET   =

# The compilation options, set to e.g., -O to run all optimizations
CIBYL_MIPS2JAVA_OPTS ?=-Iinclude
CIBYL_SYSCALL_SETS ?= ansi hello
CIBYL_GENERATE_JAVA_WRAPPERS_OPTS=-I include -S syscalls
JAVA_OPTS=

all: include/ $(SUBDIRS)

include $(CIBYL_BASE)/build/Rules-java.mk

include/: FORCE
	$(CIBYL_BASE)/tools/cibyl-generate-c-header -o $@ syscalls/ hello

FORCE:
