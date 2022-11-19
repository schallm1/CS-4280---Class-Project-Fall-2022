
all: P2

P2: P2.o
	g++ -o P2 P2.o

P2.o: P2.c parser.c scanner.c testTree.c
	g++ -c P2.c

clean:
	rm -f P2
	rm -f *.o
	rm -r -d *.dSYM
