######################################################################
##
## Copyright (C) 2008,  Simon Kagstrom
##
## Filename:      Makefile
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:   Makefile for the C++ translator
##
## $Id:$
##
######################################################################
HOST_CXX ?= g++
HOST_LD ?= g++
HOST_CFLAGS ?=-ggdb -Iinclude -Wall

SRCS=instruction.cc basicblock.cc controller.cc javamethod.cc \
     function.cc javaclass.cc mips.cc utils.cc elf.cc emit.cc \
     registerallocator.cc calltablemethod.cc codeblock.cc \
     string-instruction.cc builtins.cc syscall-wrappers.cc \
     functioncolocation.cc

OBJS=$(patsubst %.cc,objs/%.oo,$(SRCS))
DEPS=$(patsubst %.cc,deps/%.d,$(SRCS))

GCOV=-fprofile-arcs -ftest-coverage

all: $(DEPS) xcibyl-translator

-include $(DEPS)

clean:
	rm -rf objs/* deps/* *.gcda *.gcno *~ xcibyl-translator xcibyl-translator-gcov lib/libghthash-0.6.2 lib/.libghthash

objs/%.oo: %.cc
	$(HOST_CXX) $(GCOV) -Ilib/libghthash-0.6.2/src/ $(HOST_CFLAGS) -c -o $@ $<

deps/%.d: %.cc
	$(HOST_CXX) -MM -MT '$(patsubst %.cc,objs/%.oo,$<)' $(GCOV) -Ilib/libghthash-0.6.2/src/ $(HOST_CFLAGS) -o $@ $<

lib/.libghthash:
	cd lib && tar -xzf libghthash-0.6.2.tar.gz
	cd lib/libghthash-0.6.2/ && ./configure && make
	touch $@

xcibyl-translator: lib/.libghthash $(OBJS)
	$(HOST_LD) $(GCOV) -Llib/libghthash-0.6.2/src/.libs/ -o $@ $(OBJS) -lghthash -lelf -static
