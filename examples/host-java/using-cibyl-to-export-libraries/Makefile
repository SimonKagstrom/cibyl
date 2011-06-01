# Example Makefile for Cibyl application

all: program.jar

%.o: %.c
	mips-cibyl-elf-gcc -c -Os -o $@ $<

libtest.a: lib/implementation.o
	mips-cibyl-elf-ar crs $@ $<


program: exports.o libtest.a
	mips-cibyl-elf-gcc -Os -o $@ $< -L. -l test

tmpclasses/Cibyl.j: program
	cibyl-mips2java -O -d tmpclasses -DNOJ2ME program

program.jar: tmpclasses/Cibyl.j
	cp StandaloneMain.java tmpclasses
	cp LibraryWrapper.java tmpclasses
	javac -d tmpclasses -classpath tmpclasses -source 1.4 tmpclasses/*.java
	cd tmpclasses && jar cfm ../$@ ../manifest.txt *.class program.data.bin


clean:
	rm -rf tmpclasses program program.jar *.o *.a *~
