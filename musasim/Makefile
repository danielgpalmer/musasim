include common.mk

CARDSDIR=./hardware/cards/
LIBCARDS=./hardware/cards/libcards.a

all: gdbserver musasim genheader

clean:
	rm -f gdbserver musasim *.o
	$(MAKE) -C musashi clean
	$(MAKE) -C genheader clean
	$(MAKE) -C $(CARDSDIR) clean
	
BASESTUFF=sim.o board.o logging.o utils.o

gdbserver: musashi $(BASESTUFF) gdbserver.o argsgdb.o libcards.a 
	$(CC) $(LFLAGS) -D GDBSERVER musashi/musashigdb.o gdbserver.o argsgdb.o $(BASESTUFF) $(LIBCARDS) -o gdbserver

musasim: musashi $(BASESTUFF) musasim.o args.o libcards.a 
	$(CC) $(LFLAGS) musasim.o musashi/musashi.o args.o $(BASESTUFF) $(LIBCARDS) -o musasim

gdbserver.o: gdbserver.c gdbserver.h sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) --std=gnu99 gdbserver.c -o gdbserver.o

musasim.o: musasim.c sim.h musashi/m68k.h musashi/m68kconf.h
        $(CC) $(CFLAGS) --std=gnu99 musasim.c -o musasim.o

gdbserver.o: gdbserver.c gdbserver.h sim.h musashi/m68k.h musashi/m68kconf.h
        $(CC) $(CFLAGS) --std=gnu99 gdbserver.c -o gdbserver.o

sim.o: sim.c sim.h musashi/m68k.h musashi/m68kconf.h obj/board.o
	$(CC) $(CFLAGS) --std=gnu99 sim.c -o sim.o

logging.o: logging.c logging.h
	$(CC) $(CFLAGS) -std=gnu99 logging.c -o logging.o

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) utils.c -o utils.o

args.o: args.h args.c
	$(CC) $(CFLAGS) -std=gnu99 args.c -o args.o

argsgdb.o: args.h args.c
	$(CC) $(CFLAGS) -std=gnu99 -D GDBSERVER args.c -o argsgdb.o

# hardware 

board.o: hardware/board.c hardware/board.h
	$(CC) $(CFLAGS) --std=gnu99 hardware/board.c -o board.o	


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
