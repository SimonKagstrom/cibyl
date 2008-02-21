######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      Makefile
## Author:        Simon Kagstrom <ska@bth.se>
## Description:
##
## $Id: Makefile 13563 2007-02-10 08:01:26Z ska $
##
######################################################################
SUBDIRS=tools/compiled/translator libs
SYSCALL_SETS=softfloat resource_manager jmicropolygon android

all: libs/lib include/generated $(SUBDIRS)

libs/lib:
	install -d $@

include/generated: FORCE
	install -d include/generated
	$(CIBYL_BASE)/tools/cibyl-generate-c-header -o $@ $(CIBYL_BASE)/syscalls/ $(SYSCALL_SETS)

clean:
	for dir in $(SUBDIRS); do \
		if [ -f $$dir/Makefile ]; then make clean -C $$dir; fi \
	done
	make -C examples clean
	rm -rf include/generated cibyl-syscalls.db
	find . -name "*~" -or -name "*.pyc" | xargs rm -f

FORCE:

.PHONY: $(SUBDIRS)

$(SUBDIRS):
	if [ -f $@/Makefile ]; then make -C $@ ; fi
