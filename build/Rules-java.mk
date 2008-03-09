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
SOURCES   ?= tmpclasses/StandaloneMain.java tmpclasses/CRunTime.java \
             tmpclasses/Syscalls.java tmpclasses/CibylConfig.java
GEN_SYSCALL_OPTS ?=
CIBYL_JAVA_OPTS =
EXTRA_CLEAN=tmpclasses/ classes/ res/ $(SOURCES) .dirs .jar_built *~ c/syscalls.h

CIBYL_SYSCALL_DIR    ?= $(CIBYL_BASE)/syscalls
CIBYL_SYSCALL_SETS   ?= ansi
CIBYL_GENERATE_JAVA_WRAPPERS_OPTS += -D NOJ2ME
CIBYL_MIPS2JAVA_OPTS ?= -DNOJ2ME

all: $(SUBDIRS) .dirs tmpclasses/Cibyl.class res/program.data.bin $(RESOURCES) classes/.rebuilt $(TARGET)

run: all
	cd tmpclasses && $(JAVA_PATH)/java StandaloneMain ../res/program.data.bin

include $(CIBYL_BASE)/build/Rules-java-common.mk
