######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 12150 2006-11-16 08:10:57Z ska $
##
######################################################################
SUBDIRS =
OBJS    = helpers.o fileops.o memory-filesystem.o
ALL_TARGETS=install
EXTRA_CLEAN=libcrt0.a

include $(CIBYL_BASE)/build/Rules.mk

install: libcrt0.a
	cp $< $(CIBYL_BASE)/libs/lib
