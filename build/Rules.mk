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

as       ?= mips-linux-gnu-gcc
cc       ?= mips-linux-gnu-gcc
cxx      ?= mips-linux-gnu-g++
cpp      ?= mips-linux-gnu-cpp
ld       ?= mips-linux-gnu-ld
ar       ?= mips-linux-gnu-ar
jasmin   ?= jasmin

# -Wa,--no-warn inhibits the "Warning: register value used as expression"
# from the assembler on syscall arguments. It is not a valid warning for
# Cibyl, although I would wish it could be turned off only by itself.
INCLUDES += -I$(CIBYL_BASE)/include -I$(CIBYL_BASE)/include/generated -I.
ASOPTS   = -G0 -Wall -Wa,--no-warn -g -mips1 -mno-check-zero-division -mno-abicalls -fno-pic $(INCLUDES) $(ASFLAGS)
COPTS    = -G0 -DCIBYL=1 -g -msoft-float -fno-optimize-sibling-calls -nostdinc -Wall -Wa,--no-warn -mips1 -mno-check-zero-division -Os -fno-pic -mno-abicalls $(INCLUDES) $(CFLAGS) $(DEFINES)
LDLIBS  ?=
LDOPTS_DEBUG = -L$(CIBYL_BASE)/libs/lib/ -EB -nostdlib -T$(CIBYL_BASE)/build/linker.lds
LDOPTS   = $(LDOPTS_DEBUG) --emit-relocs

# User controllable
CIBYL_SYSCALL_DIR    ?= $(CIBYL_BASE)/syscalls
CIBYL_MIPS2JAVA_OPTS ?=

ALL_TARGETS ?= $(TARGET) $(TARGET).debug CompiledProgram.class

all: $(ALL_TARGETS)

$(TARGET).debug: $(OBJS)
	$(ld) $(LDOPTS_DEBUG) $+ $(CIBYL_BASE)/libs/crt0.o --start-group -lcrt0 $(LDLIBS) --end-group -o $@

$(TARGET): $(OBJS)
	$(ld) $(LDOPTS) $+ $(CIBYL_BASE)/libs/crt0.o --start-group -lcrt0 $(LDLIBS) --end-group -o $@

CompiledProgram.j: $(TARGET)
	$(CIBYL_BASE)/tools/cibyl-mips2java $(CIBYL_MIPS2JAVA_OPTS) -o $@ -d $(TARGET).data.bin -I$(CIBYL_BASE)/include/generated $<

CompiledProgram.class: CompiledProgram.j
	$(jasmin) $<

%.lsym: %
	mips-lsym-convert -o $@ $<


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

include $(CIBYL_BASE)/build/Rules-common.mk
