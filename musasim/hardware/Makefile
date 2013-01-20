include ../common.mk

all: board.o boardgdb.o registerplanner.o
	$(MAKE) -C modules
	$(MAKE) -C cards

registerplanner.o: registerplanner.c registerplanner.h
	$(CC) $(CFLAGS) --std=gnu99 $< -o $@
	
board.o: board.c board.h
	$(CC) $(CFLAGS) $(CFLAGS_GLIB) $(CFLAGS_GTHREAD) -std=gnu99 $< -o $@

boardgdb.o: board.c board.h ../gdbserver.h
	$(CC) $(CFLAGS) $(CFLAGS_GLIB) $(CFLAGS_GTHREAD) -DGDBSERVER -std=gnu99 $< -o $@


clean:
	rm -f *.o regplannertest
	$(MAKE) -C modules clean
	$(MAKE) -C cards clean
