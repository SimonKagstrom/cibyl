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
OBJS    = inputstream-filesystem.o outputstream-filesystem.o
ALL_TARGETS=install
EXTRA_CLEAN=libjava.a

include $(CIBYL_BASE)/build/Rules.mk

install: libjava.a
	cp $< $(CIBYL_BASE)/libs/lib
