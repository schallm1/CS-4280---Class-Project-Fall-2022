
all: P3

P3: P3.o
	g++ -o P3 P3.o

P3.o: P3.c parser.c scanner.c testTree.c
	g++ -c P3.c

clean:
	rm -f P3
	rm -f *.o
	rm -r -d *.dSYM
