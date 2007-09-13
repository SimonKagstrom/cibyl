######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      Rules-java.mk
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Java Makefiel rules
##
## $Id: Rules-j2me.mk 13564 2007-02-10 08:20:28Z ska $
##
######################################################################
CIBYL_SYSCALL_SETS   ?= j2me
EXTRA_CLEAN=tmpclasses/ classes/ res/ src/ $(SOURCES) .dirs .jar_built *~ c/syscalls.h

SOURCES   ?= src/Main.java src/CRunTime.java src/GameScreenCanvas.java src/Syscalls.java src/CibylConfig.java
CIBYL_JAVA_OPTS ?= -source 1.4 -bootclasspath $(WTK_PATH)/lib/cldcapi11.jar:$(WTK_PATH)/lib/midpapi20.jar
EMULATOR_OPTIONS ?=
CIBYL_GENERATE_JAVA_WRAPPERS_OPTS ?=

all: $(SUBDIRS) .dirs $(RESOURCES) classes/.preverify $(TARGET)

%.jad: %.jar
	cat $@.templ > $@
	echo -n "MIDlet-Jar-Size: " >> $@
	wc -c $< | cut -d ' ' -f -1 >> $@

%.jar: classes/.preverify
	$(JAVA_PATH)/jar cfm $@ MANIFEST.MF -C classes . -C res .
	touch .jar_built

classes/.preverify: tmpclasses/.rebuilt
	$(WTK_PATH)/bin/preverify -classpath $(WTK_PATH)/lib/cldcapi11.jar:$(WTK_PATH)/lib/midpapi20.jar:$(CIBYL_CLASSPATH) -d classes/ tmpclasses/
	touch $@

run: all
	$(WTK_PATH)/bin/emulator $(EMULATOR_OPTIONS) -classpath $(WTK_PATH)/lib/cldcapi11.jar:$(WTK_PATH)/lib/midpapi20.jar:$(wildcard *.jar):$(CIBYL_CLASSPATH) -Xdescriptor:$(wildcard *.jad)


include $(CIBYL_BASE)/build/Rules-java-common.mk
