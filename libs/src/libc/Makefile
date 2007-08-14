######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 14619 2007-03-31 05:43:51Z ska $
##
######################################################################
SUBDIRS =
OBJS    = ansi.o smalloc.o printf.o vsnprintf.o string.o dirent.o \
          stdlib.o
ALL_TARGETS=install
EXTRA_CLEAN=libc.a

include $(CIBYL_BASE)/build/Rules.mk

install: libc.a
	cp $< $(CIBYL_BASE)/libs/lib
