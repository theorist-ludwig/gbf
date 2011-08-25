PREFIX=/usr/local
CC=gcc
#CFLAGS=-std=c99 -ggdb3 -pedantic -Wall -Werror -L/usr/local/lib -lefence
CFLAGS=-std=c99 -O3 -march=athlon-xp -pedantic -Wall -Werror 

gbf: gbrainfuck.o stack.o
	$(CC) $(CFLAGS) stack.o gbrainfuck.o -o gbf

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

gbrainfuck.o: gbrainfuck.c stack.c
	$(CC) $(CFLAGS) -c gbrainfuck.c

install:
	install gbf $(PREFIX)/bin
uninstall:
	rm -f $(PREFIX)/bin/gbf
clean:
	rm -f fil gbf *.o
