######################################################################
##
## Copyright (C) 2005,  Blekinge Institute of Technology
##
## Filename:      Rules.mk
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Makefile rules for MIPS
##
## $Id: Rules.mk 14379 2007-03-21 13:43:08Z ska $
##
######################################################################
EXTRA_CLEAN += $(OBJS) $(TARGET).data.bin $(TARGET).debug CompiledProgram.class CompiledProgram.j

as       ?= mips-cibyl-elf-gcc
cc       ?= mips-cibyl-elf-gcc
cxx      ?= mips-cibyl-elf-g++
cpp      ?= mips-cibyl-elf-cpp
ld       ?= mips-cibyl-elf-ld
ar       ?= mips-cibyl-elf-ar

# -Wa,--no-warn inhibits the "Warning: register value used as expression"
# from the assembler on syscall arguments. It is not a valid warning for
# Cibyl, although I would wish it could be turned off only by itself.
INCLUDES += -I$(CIBYL_BASE)/include -I$(CIBYL_BASE)/include/generated -I.
ASOPTS   = -G0 -Wall -Wa,--no-warn -g -mips1 -mno-check-zero-division -mno-abicalls -fno-pic $(INCLUDES) $(ASFLAGS)
COPTS    = -G0 -DCIBYL=1 -ggdb -msoft-float -fno-optimize-sibling-calls -nostdinc -Wall -Wa,--no-warn -mips1 -mno-check-zero-division -Os -fno-pic -mno-abicalls $(INCLUDES) $(CFLAGS) $(DEFINES)
LDLIBS   += -lc -ljava
LDOPTS_DEBUG = -L$(CIBYL_BASE)/libs/lib/ -EB -nostdlib --whole-archive
LDOPTS   = $(LDOPTS_DEBUG) --emit-relocs

CRT0    ?= $(CIBYL_BASE)/libs/crt0.o

ALL_TARGETS ?= $(TARGET)
# $(TARGET).debug

all: $(ALL_TARGETS)

$(TARGET).debug: $(OBJS)
	$(ld) $(LDOPTS_DEBUG) $+ $(CRT0) $(CIBYL_BASE)/libs/crt0-qemu-debug.o -T$(CIBYL_BASE)/build/linker-qemu.lds --start-group -lcrt0 $(LDLIBS) -ldebug --end-group -o $@

$(TARGET): $(OBJS)
	$(ld) $(LDOPTS) $+ $(CRT0) -T$(CIBYL_BASE)/build/linker.lds --start-group -lcrt0 $(LDLIBS) --end-group -o $@


%.a: $(OBJS)
	$(ar) -crs $@ $+

%.o: %.S
	$(as) $(ASOPTS) -c $< -o $@

%.o: %.s
	$(as) $(ASOPTS) -c $< -o $@

%.o: %.c
	$(cc) $(COPTS) -c $< -o $@

%.oo: %.cc
	$(cxx) -fno-exceptions -fno-rtti $(COPTS) -c $< -o $@

%.oo: %.cpp
	$(cxx) -fno-exceptions -fno-rtti $(COPTS) -c $< -o $@

%.oo: %.cxx
	$(cxx) -fno-exceptions -fno-rtti $(COPTS) -c $< -o $@

include $(CIBYL_BASE)/build/Rules-common.mk
