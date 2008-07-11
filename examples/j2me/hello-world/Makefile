######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 9745 2006-09-04 18:04:52Z ska $
##
######################################################################
SUBDIRS = c
RESOURCES=res/program.data.bin
TARGET   =midlet.jar midlet.jad

# This is actually not needed since we include Rules-j2me, but
# we keep it here to show how new syscall sets are added. Further,
# ansi is implicitly implied by j2me, so that can be removed as well
CIBYL_SYSCALL_SETS ?= javax.microedition ansi

include $(CIBYL_BASE)/build/Rules-j2me.mk
