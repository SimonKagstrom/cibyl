######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 12755 2006-12-14 17:55:01Z ska $
##
######################################################################
SUBDIRS =
OBJS    = math.o
ALL_TARGETS=install
EXTRA_CLEAN=libm.a

include $(CIBYL_BASE)/build/Rules.mk

install: libm.a
	cp $< $(CIBYL_BASE)/libs/lib
