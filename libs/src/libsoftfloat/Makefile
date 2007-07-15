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
EXTRA_CLEAN=libsoftfloat.a

include $(CIBYL_BASE)/build/Rules.mk

# libsoftfloat.a is actually not an ar archive. We do this because we
# want to override the (weak) vsnprintf in vsnprintf.c with a version
# in soft-float.c which has support for the %f format.
#
# See the thread  http://sourceware.org/ml/binutils/2003-10/msg00377.html
# for why we cannot use an ar archive
libsoftfloat.a: soft-float.o
	cp $< $@

install: libsoftfloat.a
	cp $< $(CIBYL_BASE)/libs/lib
