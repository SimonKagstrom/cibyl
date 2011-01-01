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
HOST_CC ?= gcc
HOST_LD ?= g++
HOST_CFLAGS ?=-ggdb -Iinclude -Ilib/dwarves/ -Ilib/dwarves/include/ -Wall

ERROR_FILTER := 2>&1 | sed -e 's/\(.[a-zA-Z]\+\):\([0-9]\+\):/\1(\2):/g'

SRCS=instruction.cc basicblock.cc controller.cc javamethod.cc \
     function.cc javaclass.cc mips.cc utils.cc elf.cc emit.cc \
     registerallocator.cc calltablemethod.cc codeblock.cc \
     string-instruction.cc builtins.cc syscall-wrappers.cc \
     functioncolocation.cc
     
SRCS_C=mips-dwarf.c

OBJS=$(patsubst %.cc,objs/%.oo,$(SRCS))
OBJS_C=$(patsubst %.c,objs/%.o,$(SRCS_C))
DEPS=$(patsubst %.cc,deps/%.d,$(SRCS))
DEPS_c=$(patsubst %.c,deps/%.d,$(SRCS_C))

#GCOV=-fprofile-arcs -ftest-coverage

all: create-dirs $(DEPS) xcibyl-translator

-include $(DEPS) $(DEPS_C)

create-dirs:
	@mkdir -p deps objs

clean:
	rm -rf objs/* deps/* *.gcda *.gcno *~ xcibyl-translator xcibyl-translator-gcov lib/libghthash-0.6.2 lib/.libghthash

objs/%.oo: %.cc
	@echo CXX $(notdir $<)
	@$(HOST_CXX) $(GCOV) -Ilib/libghthash-0.6.2/src/ $(HOST_CFLAGS) -c -o $@ $< $(ERROR_FILTER)

objs/%.o: %.c
	@echo CXX $(notdir $<)
	@$(HOST_CC) $(GCOV) -Ilib/libghthash-0.6.2/src/ $(HOST_CFLAGS) -c -o $@ $< $(ERROR_FILTER)


deps/%.d: %.cc
	@$(HOST_CXX) -MM -MT '$(patsubst %.cc,objs/%.oo,$<)' $(GCOV) -Ilib/libghthash-0.6.2/src/ $(HOST_CFLAGS) -o $@ $<

deps/%.d: %.c
	@$(HOST_CC) -MM -MT '$(patsubst %.cc,objs/%.oo,$<)' $(GCOV) -Ilib/libghthash-0.6.2/src/ $(HOST_CFLAGS) -o $@ $<

lib/.libghthash:
	cd lib && tar -xzf libghthash-0.6.2.tar.gz
	cd lib/libghthash-0.6.2/ && ./configure && make
	touch $@

lib/.dwarves:
	cd lib/dwarves/ && make
	touch $@

xcibyl-translator: lib/.libghthash $(OBJS) $(OBJS_C)
	$(HOST_LD) $(GCOV) -Llib/libghthash-0.6.2/src/.libs/ -o $@ $(OBJS) $(OBJS_C) -lghthash -ldw -lelf -static
