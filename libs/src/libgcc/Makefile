######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 12918 2007-01-07 18:10:31Z ska $
##
######################################################################
SUBDIRS =
OBJS    = soft-float.o
ALL_TARGETS=install
EXTRA_CLEAN=libgcc.a

include $(CIBYL_BASE)/build/Rules.mk

libgcc.a: soft-float.o 64-bit-ops.o cache.o

install: libgcc.a
	cp $< $(CIBYL_BASE)/libs/lib
