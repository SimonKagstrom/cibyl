######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 15765 2007-07-15 13:48:01Z ska $
##
######################################################################
SUBDIRS =
TARGET  = program
DEFINES = -DVERBOSE=0
SOURCES = src/CRunTime.java src/Main.java src/Syscalls.java
INCLUDES=-I. -I../include -fno-inline-functions
OBJS    = main.o run-tests.o console.o \
          tests/int.o tests/char.o tests/float.o tests/double.o tests/j2me.o \
          tests/varargs.o tests/libm.o tests/memory.o tests/java-exceptions.o \
	  tests/unsigned-int.o tests/file-operations.o tests/jr.o tests/jr_test.o \
          tests/relocation.o
LDLIBS  = -lm -lsoftfloat -lj2me -ljsr075

# The compilation options, set to e.g., -O to run all optimizations
CIBYL_MIPS2JAVA_OPTS +=-I../include

include $(CIBYL_BASE)/build/Rules.mk

program.host: $(patsubst %.o,%.host.o,$(OBJS))
	gcc -o $@ $+ -lm

%.host.o: %.c
	gcc -msse -mfpmath=sse -DVERBOSE=1 -I. -std=c99 -DHOST=1 -c -Os -Wall -o $@ $<
