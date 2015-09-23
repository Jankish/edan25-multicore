CC = gcc
CFLAGS = -Wall -pedantic -std=c99
SOURCE = sort.c
EXE = sort

build: $(SOURCE)
	$(CC) -o $(EXE) $(SOURCE) $(CFLAGS)

clean:
	rm -f *.o core

rebuild: clean build
