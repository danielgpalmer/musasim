include ../common.mk

all: board.o

board.o: board.c board.h
	$(CC) $(CFLAGS) -std=gnu99 board.c -o board.o

clean:
	rm *.o
