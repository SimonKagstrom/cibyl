# Example Makefile for Cibyl application

all: program.jar

include/cibyl-syscalls.db: syscalls/hello/include/hello.h
	cibyl-generate-c-header -o include syscalls hello

program: main.c include/cibyl-syscalls.db
	mips-cibyl-elf-gcc -I include -Os -o $@ $<


tmpclasses/Cibyl.j: program
	cibyl-mips2java -I include -O -d tmpclasses -DNOJ2ME program

program.jar: tmpclasses/Cibyl.j
	cp `cibyl-config --prefix`/mips-cibyl-elf/sys-root/usr/share/java/StandaloneMain.java tmpclasses
	cd tmpclasses && javac -source 1.4 *.java
	cd tmpclasses && jar cfm ../$@ ../manifest.txt *.class program.data.bin


clean:
	rm -rf tmpclasses program program.jar include *~
