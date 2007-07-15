######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      Rules-java.mk
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Java Makefiel rules
##
## $Id: Rules-java.mk 14155 2007-03-11 09:43:00Z ska $
##
######################################################################
SOURCES   ?= src/StandaloneMain.java src/CRunTime.java src/Syscalls.java src/CibylConfig.java
GEN_SYSCALL_OPTS ?=
JAVA_OPTS ?=
EXTRA_CLEAN=tmpclasses/ classes/ res/ src/ $(SOURCES) .dirs .jar_built *~ c/syscalls.h

CIBYL_SYSCALL_DIR    ?= $(CIBYL_BASE)/syscalls
CIBYL_SYSCALL_SETS   ?= ansi
CIBYL_GENERATE_JAVA_WRAPPERS_OPTS += -D NOJ2ME

all: $(SUBDIRS) .dirs $(RESOURCES) src/CompiledProgram.class classes/.rebuilt $(TARGET)

run: all
	cd tmpclasses && $(JAVA_PATH)/java StandaloneMain ../res/program.data.bin

include $(CIBYL_BASE)/build/Rules-java-common.mk
