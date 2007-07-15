######################################################################
##
## Copyright (C) 2006,  Blekinge Institute of Technology
##
## Filename:      Rules-common.mk
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Common Makefile rules
##
## $Id: Rules-common.mk 13545 2007-02-09 21:16:15Z ska $
##
######################################################################

all: $(EXTRA_TARGETS) $(SUBDIRS) $(TARGET)

clean:
	for dir in $(SUBDIRS); do \
		if [ -f $$dir/Makefile ]; then make clean -C $$dir; fi \
	done
	rm -rf $(TARGET) $(EXTRA_CLEAN) *~

# Rule for making subdirectories
.PHONY: $(SUBDIRS)

$(SUBDIRS):
	if [ -f $@/Makefile ]; then make -C $@ ; fi
