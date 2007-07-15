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
TARGET   =src/CompiledProgram.class CibylTest.jar CibylTest.jad
CIBYL_SYSCALL_SETS ?= resource_manager softfloat

CIBYL_GENERATE_JAVA_WRAPPERS_OPTS=-DJSR075
CIBYL_CLASSPATH=$(WTK_PATH)/lib/jsr75.jar

include $(CIBYL_BASE)/build/Rules-j2me.mk
