######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 13543 2007-02-09 21:04:04Z ska $
##
######################################################################
SUBDIRS =
TARGET  = program
SOURCES = src/CRunTime.java src/Main.java src/Syscalls.java
OBJS    = main.o
INCLUDES=-I../include/
LDLIBS  =

# The compilation options, set to e.g., -O to run all optimizations
CIBYL_MIPS2JAVA_OPTS ?=-I../include

include $(CIBYL_BASE)/build/Rules.mk
