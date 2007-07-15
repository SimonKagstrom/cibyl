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
TARGET   =src/CompiledProgram.class

CIBYL_SYSCALL_SETS ?= ansi
JAVA_OPTS=

include $(CIBYL_BASE)/build/Rules-java.mk

res/program.data.bin: c/program.data.bin
	cp $< $@
