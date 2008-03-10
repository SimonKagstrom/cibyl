######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 15754 2007-07-14 08:04:34Z ska $
##
######################################################################
SUBDIRS = c
RESOURCES=res/program.data.bin res/file-which-exists res/b
TARGET   =CibylTest.jar CibylTest.jad
CIBYL_SYSCALL_SETS ?= resource_manager softfloat command_mgr

CIBYL_GENERATE_JAVA_WRAPPERS_OPTS=-DJSR075 -I include -S syscalls
CIBYL_CLASSPATH=$(WTK_PATH)/lib/jsr75.jar
# The compilation options, set to e.g., -O to run all optimizations
CIBYL_MIPS2JAVA_OPTS +=-Iinclude -DJSR075

all: include/ $(SUBDIRS)

include $(CIBYL_BASE)/build/Rules-j2me.mk

include/: FORCE
	$(CIBYL_BASE)/tools/cibyl-generate-c-header -o $@ syscalls/ command_mgr
FORCE:
