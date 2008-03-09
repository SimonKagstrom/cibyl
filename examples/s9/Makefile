######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 15431 2007-05-19 11:54:12Z ska $
##
######################################################################
SUBDIRS = c
RESOURCES=res/program.data.bin
TARGET   =include/ tmpclasses/CibylConfig.java midlet.jar midlet.jad

# This is actually not needed since we include Rules-j2me, but
# we keep it here to show how new syscall sets are added. Further,
# ansi is implicitly implied by j2me, so that can be removed as well
CIBYL_SYSCALL_SETS ?= javax.microedition ansi resource_manager command_mgr
CIBYL_GENERATE_JAVA_WRAPPERS_OPTS=-S syscalls -I include
CIBYL_MIPS2JAVA_OPTS +=-Iinclude

all: include

include $(CIBYL_BASE)/build/Rules-j2me.mk

tmpclasses/CibylConfig.java: CibylConfig.java
	cp $< $@

include/: FORCE
	$(CIBYL_BASE)/tools/cibyl-generate-c-header -o $@ syscalls/ command_mgr
FORCE:
