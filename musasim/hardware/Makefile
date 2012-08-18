include ../common.mk

all: board.o boardgdb.o
	$(MAKE) -C modules
	$(MAKE) -C cards

board.o: board.c board.h
	$(CC) $(CFLAGS) -std=gnu99 $< -o $@

boardgdb.o: board.c board.h
	$(CC) $(CFLAGS) -DGDBSERVER -std=gnu99 $< -o $@

clean:
	rm -f *.o
	$(MAKE) -C modules clean
	$(MAKE) -C cards clean
