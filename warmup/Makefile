CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic -fsanitize=address

all: split

split: main.o split.o
	gcc $(CFLAGS) -o split main.o split.o

main.o: main.c split.h
	gcc $(CFLAGS) -c main.c

split.o: split.c split.h
	gcc $(CFLAGS) -c split.c

clean:
	rm -f *.o split