# Example Makefile for Cibyl application

# Find out the path to the wireless toolkit
PREVERIFY_BINARY_PATH=$(shell which preverify)
PREVERIFY_REAL_PATH=$(shell readlink -f $(PREVERIFY_BINARY_PATH))
PREVERIFY_DIRNAME=$(shell dirname $(PREVERIFY_BINARY_PATH))
WTK_PATH=$(PREVERIFY_DIRNAME)/../


OBJS=src/main.o

all: midlet.jad

%.o: %.c
	mips-cibyl-elf-gcc -c -Os -o $@ $<

program: $(OBJS)
	mips-cibyl-elf-gcc -Os -o $@ $<

tmpclasses/Cibyl.j: program
	cibyl-mips2java -O -d tmpclasses program


midlet.jar: tmpclasses/Cibyl.j
	install -d res
	cp `cibyl-config --prefix`/mips-cibyl-elf/sys-root/usr/share/java/Main.java tmpclasses
	cp `cibyl-config --prefix`/mips-cibyl-elf/sys-root/usr/share/java/GameScreenCanvas.java tmpclasses
	javac -d tmpclasses -classpath tmpclasses -source 1.4 -bootclasspath $(WTK_PATH)/lib/cldcapi11.jar:$(WTK_PATH)/lib/midpapi20.jar -source 1.4 tmpclasses/*.java
	preverify -classpath $(WTK_PATH)/lib/cldcapi11.jar:$(WTK_PATH)/lib/midpapi20.jar -d classes tmpclasses
	cp tmpclasses/program.data.bin res
	cp resources/* res/
	jar cfm $@ MANIFEST.MF -C classes . -C res .

midlet.jad: midlet.jad.templ midlet.jar
	cat $< > $@
	echo -n "MIDlet-Jar-Size: " >> $@
	wc -c midlet.jar | cut -d ' ' -f -1 >> $@


clean:
	rm -rf classes tmpclasses program *.jar *.jad *~ res
