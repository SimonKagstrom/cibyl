######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 12466 2006-11-25 18:59:40Z ska $
##
######################################################################
SUBDIRS =
TARGET  = program
OBJS    = lib/implementation.o exports.o
LDLIBS  =

# The compilation options, set to e.g., -O to run all optimizations
CIBYL_MIPS2JAVA_OPTS ?=

include $(CIBYL_BASE)/build/Rules.mk

lib/libtest.a: lib/implementation.o
	ar crs $@ $<

program: lib/libtest.a exports.o
	$(ld) $(LDOPTS) exports.o -ltest -Llib $(CRT0) -T$(CIBYL_BASE)/build/linker.lds --start-group -lcrt0 $(LDLIBS) --end-group -o $@
