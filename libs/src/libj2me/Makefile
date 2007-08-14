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
OBJS    = resource-filesystem.o recordstore-filesystem.o \
          inputstream-filesystem.o connector-filesystem.o
ALL_TARGETS=install
EXTRA_CLEAN=libj2me.a

include $(CIBYL_BASE)/build/Rules.mk

install: libj2me.a
	cp $< $(CIBYL_BASE)/libs/lib
