include ../common.mk

all: board.o boardgdb.o registerplanner.o
	$(MAKE) -C modules
	$(MAKE) -C cards

registerplanner.o: registerplanner.c registerplanner.h
	$(CC) $(CFLAGS) --std=gnu99 $< -o $@

regplannertester: regplannertester.c registerplanner.o
	$(CC) --std=gnu99 $^ ../utils.o -o $@ 

board.o: board.c board.h
	$(CC) $(CFLAGS) -std=gnu99 $< -o $@

boardgdb.o: board.c board.h ../gdbserver.h
	$(CC) $(CFLAGS) -DGDBSERVER -std=gnu99 $< -o $@


clean:
	rm -f *.o regplannertest
	$(MAKE) -C modules clean
	$(MAKE) -C cards clean
