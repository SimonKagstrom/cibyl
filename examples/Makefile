######################################################################
##
## Copyright (C) 2006-2008,  Simon Kågström
##
## Filename:      Makefile
## Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
## Description:
##
## $Id: Makefile 15431 2007-05-19 11:54:12Z ska $
##
######################################################################
SUBDIRS=host-java/hello-world host-java/floating-point host-java/cpp-test \
        host-java/exporting-java-functionality-to-cibyl host-java/file-ops \
        host-java/using-cibyl-to-export-libraries \
        j2me/hello-world j2me/tiles-sprites j2me/sokoban j2me/tafl-client j2me/events j2me/s9

all: $(SUBDIRS)

clean:
	for dir in $(SUBDIRS); do \
		if [ -f $$dir/Makefile ]; then make clean -C $$dir; fi \
	done
	rm -rf $(TARGET) $(EXTRA_CLEAN) *~

# Rule for making subdirectories
.PHONY: $(SUBDIRS)

$(SUBDIRS):
	if [ -f $@/Makefile ]; then make -C $@ ; fi
