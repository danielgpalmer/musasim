include ../common.mk

all: board.o
	$(MAKE) -C cards

board.o: board.c board.h
	$(CC) $(CFLAGS) -std=gnu99 board.c -o board.o

clean:
	rm *.o
