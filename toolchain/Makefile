TGT=x-tools

all: make_tarball

.PHONY: crosstool-ng-1.9.1 crosstool_install toolchain_build clean_install \
	install_replacements do_chmod make_tarball clean

crosstool-ng-1.9.1.tar.bz2:
	wget http://ymorin.is-a-geek.org/download/crosstool-ng/$@

crosstool-ng-1.9.1: crosstool-ng-1.9.1.tar.bz2
	tar -xjf $<

crosstool_install: crosstool-ng-1.9.1
	cd $< && ./configure --prefix=`pwd`/../$@ && make && make install

toolchain_build: ct-ng.config crosstool_install
	install -d $@
	cp ct-ng.config $@/.config
	cd $@ && ../crosstool_install/bin/ct-ng build

do_chmod: toolchain_build
	chmod -R u+rw ${TGT}

clean_install: do_chmod
	rm -rf ${TGT}/mips-cibyl-elf/mips-cibyl-elf/include
	rm -rf ${TGT}/mips-cibyl-elf/sys-root/lib/*
	rm -rf ${TGT}/mips-cibyl-elf/sys-root/usr/include/*
	rm -rf ${TGT}/mips-cibyl-elf/share/man
	rm -rf ${TGT}/mips-cibyl-elf/lib/ldscripts/elf32ebmip*

jasmin-2.4.zip:
	wget http://downloads.sourceforge.net/project/jasmin/jasmin/jasmin-2.4/$@

jasmin-2.4: jasmin-2.4.zip
	unzip $<

install_replacements: clean_install jasmin-2.4
	mv ${TGT}/mips-cibyl-elf/mips-cibyl-elf/bin/ld ${TGT}/mips-cibyl-elf/mips-cibyl-elf/bin/ld.real
	mv ${TGT}/mips-cibyl-elf/bin/mips-cibyl-elf-gcc ${TGT}/mips-cibyl-elf/bin/mips-cibyl-elf-gcc.real
	mv ${TGT}/mips-cibyl-elf/bin/mips-cibyl-elf-g++ ${TGT}/mips-cibyl-elf/bin/mips-cibyl-elf-g++.real
	mv ${TGT}/mips-cibyl-elf/bin/mips-cibyl-elf-ld ${TGT}/mips-cibyl-elf/bin/mips-cibyl-elf-ld.real
	install ld ${TGT}/mips-cibyl-elf/mips-cibyl-elf/bin/
	install mips-cibyl-elf-gcc ${TGT}/mips-cibyl-elf/bin/
	install mips-cibyl-elf-g++ ${TGT}/mips-cibyl-elf/bin/
	install mips-cibyl-elf-ld ${TGT}/mips-cibyl-elf/bin/
	install cibyl-jasmin ${TGT}/mips-cibyl-elf/bin/
	install jasmin-2.4/jasmin.jar ${TGT}/mips-cibyl-elf/lib

make_tarball: install_replacements
	cd ${TGT} && tar -cjf ../mips-cibyl-elf.tar.bz2 mips-cibyl-elf

# Remove everything except crosstool
clean:
	rm -rf crosstool-ng-1.9.1 crosstool_install x-tools toolchain_build jasmin-2.4
	rm -rf mips-cibyl-elf.tar.bz2
