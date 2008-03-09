######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 14966 2007-04-16 14:37:30Z ska $
##
######################################################################
SUBDIRS = c
RESOURCES=res/program.data.bin
TARGET   =tmpclasses/CibylConfig.java midlet.jar midlet.jad

# This is actually not needed since we include Rules-j2me, but
# we keep it here to show how new syscall sets are added. Further,
# ansi is implicitly implied by j2me, so that can be removed as well
CIBYL_SYSCALL_SETS ?= javax.microedition ansi

include $(CIBYL_BASE)/build/Rules-j2me.mk

tmpclasses/CibylConfig.java: CibylConfig.java
	cp $< $@
