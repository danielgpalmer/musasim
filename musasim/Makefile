include common.mk

LIBCARDS=./hardware/cards/libcards.a
LOCALCFLAGS=--std=gnu99 `pkg-config --cflags $(ALLEXTLIBS)`
BASESTUFF=sim.o logging.o utils.o

all: gdbserver musasim genheader dist
	
gdbserver: musashi $(BASESTUFF) hardware gdbserver.o argsgdb.o
	$(CC) $(LFLAGS) -D GDBSERVER musashi/musashigdb.o gdbserver.o argsgdb.o $(BASESTUFF) $(LIBCARDS) ./hardware/board.o -o gdbserver

musasim: musashi $(BASESTUFF) hardware musasim.o args.o 
	$(CC) $(LFLAGS) musasim.o musashi/musashi.o args.o $(BASESTUFF) $(LIBCARDS) ./hardware/board.o -o musasim

gdbserver.o: gdbserver.c gdbserver.h sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) gdbserver.c -o gdbserver.o

musasim.o: musasim.c sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) musasim.c -o musasim.o

gdbserver.o: gdbserver.c gdbserver.h sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) gdbserver.c -o gdbserver.o

sim.o: sim.c sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) sim.c -o sim.o

logging.o: logging.c logging.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) logging.c -o logging.o

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) $(LOCALCLAGS) utils.c -o utils.o

args.o: args.h args.c
	$(CC) $(CFLAGS) $(LOCALCLAGS) args.c -o args.o

argsgdb.o: args.h args.c
	$(CC) $(CFLAGS) $(LOCALCLAGS) -D GDBSERVER args.c -o argsgdb.o

# hardware 

.PHONY: docs genheader musashi hardware clean

hardware:
	$(MAKE) -C hardware

musashi:
	$(MAKE) -C musashi

genheader:
	$(MAKE) -C genheader

docs:
	rm -rf ../docs/html
	doxygen DoxyFile

dist:	musasim gdbserver
	mkdir dist
	cp musasim gdbserver dist/
	strip dist/musasim
	strip dist/gdbserver
	tar czf musasim.tar.gz dist/
	rm -r dist

clean:
	rm -f gdbserver musasim *.o
	$(MAKE) -C musashi clean
	$(MAKE) -C genheader clean
	$(MAKE) -C hardware clean
