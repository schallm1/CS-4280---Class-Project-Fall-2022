
all: P1

P1: P1.o
	g++ -o P1 P1.o

P1.o: P1.c
	g++ -c P1.c

clean:
	rm -f P1
	rm -f P1.o
