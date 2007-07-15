######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 12888 2007-01-06 08:59:23Z ska $
##
######################################################################
SUBDIRS =
TARGET  = program
SOURCES = src/CRunTime.java src/Main.java src/Syscalls.java
OBJS    = main.o
LDLIBS  = -lc -lsoftfloat -lm

# The compilation options, set to e.g., -O to run all optimizations
CIBYL_MIPS2JAVA_OPTS ?=

include $(CIBYL_BASE)/build/Rules.mk
