include common.mk

CARDSDIR=./hardware/cards/
LIBCARDS=./hardware/cards/libcards.a

all: obj gdbserver musasim genheader

clean:
	rm -rf obj
	rm -f gdbserver
	rm -f musasim
	$(MAKE) -C musashi clean
	$(MAKE) -C genheader clean
	$(MAKE) -C $(CARDSDIR) clean
	
obj:
	mkdir obj

gdbserver: musashi obj/gdbserver.o obj/sim.o obj/logging.o obj/utils.o obj/argsgdb.o libcards.a 
	$(CC) $(LFLAGS) -D GDBSERVER musashi/musashigdb.o obj/gdbserver.o obj/sim.o obj/board.o obj/logging.o obj/utils.o obj/argsgdb.o $(LIBCARDS) -o gdbserver

musasim: musashi musasim.o obj/sim.o obj/logging.o obj/utils.o obj/args.o libcards.a 
	$(CC) $(LFLAGS) musasim.o obj/sim.o musashi/musashi.o obj/board.o obj/logging.o obj/utils.o obj/args.o $(LIBCARDS) -o musasim

obj/gdbserver.o: gdbserver.c gdbserver.h sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) --std=gnu99 gdbserver.c -o obj/gdbserver.o

musasim.o: musasim.c sim.h musashi/m68k.h musashi/m68kconf.h
        $(CC) $(CFLAGS) --std=gnu99 musasim.c -o musasim.o

obj/gdbserver.o: gdbserver.c gdbserver.h sim.h musashi/m68k.h musashi/m68kconf.h
        $(CC) $(CFLAGS) --std=gnu99 gdbserver.c -o obj/gdbserver.o

obj/sim.o: sim.c sim.h musashi/m68k.h musashi/m68kconf.h obj/board.o
	$(CC) $(CFLAGS) --std=gnu99 sim.c -o obj/sim.o

obj/logging.o: logging.c logging.h
	$(CC) $(CFLAGS) -std=gnu99 logging.c -o obj/logging.o

obj/utils.o: utils.c utils.h
	$(CC) $(CFLAGS) utils.c -o obj/utils.o

obj/args.o: args.h args.c
	$(CC) $(CFLAGS) -std=gnu99 args.c -o obj/args.o

obj/argsgdb.o: args.h args.c
	$(CC) $(CFLAGS) -std=gnu99 -D GDBSERVER args.c -o obj/argsgdb.o

# hardware 

obj/board.o: hardware/board.c hardware/board.h
	$(CC) $(CFLAGS) --std=gnu99 hardware/board.c -o obj/board.o	


.PHONY: libcards.a docs genheader musashi
	
musashi:
	$(MAKE) -C musashi

libcards.a:
	$(MAKE) -C $(CARDSDIR) libcards.a

genheader:
	$(MAKE) -C genheader

docs:
	rm -rf ../docs/html
	doxygen DoxyFile
