######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      Rules-java.mk
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Java Makefiel rules
##
## $Id: Rules-java-common.mk 13546 2007-02-09 21:16:26Z ska $
##
######################################################################
JAVA_PATH ?= /usr/lib/j2sdk1.5-sun/bin

CIBYL_SYSCALL_DIR    ?= $(CIBYL_BASE)/syscalls
CIBYL_SYSCALL_SETS   ?= ansi

.dirs:
	install -d src
	install -d res
	install -d tmpclasses
	install -d classes
	touch .dirs

res/%: resources/%
	cp `pwd`/$< `pwd`/$@

# The program text
res/%.data.bin: c/%.data.bin
	cp $< $@

src/%: $(CIBYL_BASE)/java/%
	cp $< $@

# Special cases
src/Syscalls.java: c/program
	$(CIBYL_BASE)/tools/cibyl-generate-java-wrappers -o `dirname $@` $(CIBYL_GENERATE_JAVA_WRAPPERS_OPTS) -I $(CIBYL_BASE)/include/generated -S $(CIBYL_SYSCALL_DIR) c/program $(CIBYL_SYSCALL_SETS)

src/CompiledProgram.class: c/CompiledProgram.class
	cp $< $@
	cp $< tmpclasses/

classes/.rebuilt: tmpclasses/.rebuilt
	cp tmpclasses/*.class classes/
	touch $@

tmpclasses/.rebuilt: $(SOURCES)
	cd src && $(JAVA_PATH)/javac $(JAVA_OPTS) -d ../tmpclasses/ *.java
	touch $@

include $(CIBYL_BASE)/build/Rules-common.mk
