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
OBJS    = s9.o
ALL_TARGETS=install
EXTRA_CLEAN=libs9.a *.host.o host-test

include $(CIBYL_BASE)/build/Rules.mk

install: libs9.a
	cp $< $(CIBYL_BASE)/libs/lib

host-test: s9.c main.c
	gcc -I$(CIBYL_BASE)/include -Wall -g -c -o s9.host.o s9.c
	gcc -I$(CIBYL_BASE)/include -Wall -g -c -o main.host.o main.c
	gcc -o $@ s9.host.o main.host.o