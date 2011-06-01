# Example Makefile for Cibyl application

all: program.jar

program: main.c
	mips-cibyl-elf-gcc -Os -o $@ $< -lm

tmpclasses/Cibyl.j: program
	cibyl-mips2java -O -d tmpclasses -DNOJ2ME program

program.jar: tmpclasses/Cibyl.j
	cp `cibyl-config --prefix`/mips-cibyl-elf/sys-root/usr/share/java/StandaloneMain.java tmpclasses
	javac -d tmpclasses -classpath tmpclasses -source 1.4 tmpclasses/*.java
	cd tmpclasses && jar cfm ../$@ ../manifest.txt *.class program.data.bin


clean:
	rm -rf tmpclasses program program.jar *~
