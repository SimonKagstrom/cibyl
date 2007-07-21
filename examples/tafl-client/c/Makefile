######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Generic Makefile
##
## $Id: Makefile 12466 2006-11-25 18:59:40Z ska $
##
######################################################################
SUBDIRS =
INCLUDES=-Iinclude
TARGET  = program
OBJS    = main.o gui.o game.o playfield.o utils.o move.o item.o ai.o dir.o j2me-serialization.o
LDLIBS  = -lc

# The compilation options, set to e.g., -O to run all optimizations
CIBYL_MIPS2JAVA_OPTS ?=

include $(CIBYL_BASE)/build/Rules.mk
