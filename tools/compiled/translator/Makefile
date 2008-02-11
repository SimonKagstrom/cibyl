######################################################################
##
## Copyright (C) 2008,  Simon Kagstrom
##
## Filename:      Makefile
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   stuff
##
## $Id:$
##
######################################################################
HOST_CXX ?= g++
HOST_LD ?= g++
HOST_CFLAGS ?=-ggdb -Iinclude -Wall

SRCS=instruction.cc basicblock.cc controller.cc javamethod.cc \
     function.cc javaclass.cc mips.cc utils.cc elf.cc emit.cc \
     registerallocator.cc calltablemethod.oo codeblock.oo \
     string-instruction.cc

OBJS=$(patsubst %.cc,%.oo,$(SRCS))

all: xcibyl-translator

clean:
	rm -f *.oo

%.oo: %.cc
	$(HOST_CXX) $(HOST_CFLAGS) -c -o $@ $<

xcibyl-translator: $(OBJS)
	echo $(OBJS)
	$(HOST_LD) -o $@ $+ -lghthash -lelf
