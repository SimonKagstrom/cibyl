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
     registerallocator.cc calltablemethod.oo codeblock.oo \
     string-instruction.cc builtins.cc syscall-wrappers.oo

OBJS=$(patsubst %.cc,%.oo,$(SRCS))

all: xcibyl-translator

clean:
	rm -rf *.oo *~ xcibyl-translator lib/libghthash-0.6.2 lib/.libghthash

instruction.oo: instructions/fpu.cc instructions/memory.cc instructions/rfmt.cc \
                instructions/ifmt.cc instructions/muldiv.cc instructions/set.cc \
		instructions/nop.cc instructions/syscall.cc instructions/jfmt.cc \
                include/utils.h include/instruction.hh include/entity.hh include/elf.hh \
		include/config.hh include/mips.hh

controller.oo: controller.cc \
		include/utils.h include/instruction.hh include/entity.hh include/elf.hh \
		include/config.hh include/javamethod.hh include/function.hh \
                include/javaclass.hh

function.oo: function.cc include/basicblock.hh \
		include/utils.h include/instruction.hh include/entity.hh include/elf.hh \
		include/config.hh include/function.hh

javamethod.oo: javamethod.cc include/basicblock.hh \
		include/utils.h include/instruction.hh include/entity.hh include/elf.hh \
		include/config.hh include/javamethod.hh include/function.hh

javaclass.oo: javaclass.cc \
		include/config.hh include/javaclass.hh


builtins.oo: builtins.cc builtins/exceptions.cc builtins/softfloat.cc \
		include/builtins.hh

%.oo: %.cc
	$(HOST_CXX) -Ilib/libghthash-0.6.2/src/ $(HOST_CFLAGS) -c -o $@ $<

lib/.libghthash:
	cd lib && tar -xzf libghthash-0.6.2.tar.gz
	cd lib/libghthash-0.6.2/ && ./configure && make
	touch $@

xcibyl-translator: lib/.libghthash $(OBJS)
	echo $(OBJS)
	$(HOST_LD) -Llib/libghthash-0.6.2/src/.libs/ -o $@ $(OBJS) -lghthash -lelf -static
