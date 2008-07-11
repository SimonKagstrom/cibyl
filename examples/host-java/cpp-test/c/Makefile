######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 15374 2007-05-13 07:55:15Z ska $
##
######################################################################
SUBDIRS =
TARGET  = program
SOURCES = src/CRunTime.java src/Main.java src/Syscalls.java
OBJS    = main.oo
LDLIBS  = -lstdc++

# The compilation options, set to e.g., -O to run all optimizations
CIBYL_MIPS2JAVA_OPTS ?=

include $(CIBYL_BASE)/build/Rules.mk
