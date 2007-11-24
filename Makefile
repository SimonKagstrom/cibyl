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
SUBDIRS=libs

all: libs/lib include/generated $(SUBDIRS)

libs/lib:
	install -d $@

include/generated: FORCE
	install -d include/generated
	$(CIBYL_BASE)/tools/cibyl-generate-c-header -o $@ $(CIBYL_BASE)/syscalls/ softfloat resource_manager jmicropolygon android

clean:
	make -C $(SUBDIRS) clean
	make -C examples clean
	rm -rf include/generated
	find . -name "*~" -or -name "*.pyc" | xargs rm -f

FORCE:

.PHONY: $(SUBDIRS)

$(SUBDIRS):
	if [ -f $@/Makefile ]; then make -C $@ ; fi
