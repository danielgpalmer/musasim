include common.mk

LIBCARDS=./hardware/cards/libcards.a
LIBCARDSGDB=./hardware/cards/libcardsgdb.a
LIBMODULES=./hardware/modules/libmodules.a
LOCALCFLAGS=--std=gnu99 `pkg-config --cflags $(ALLEXTLIBS)`
BASESTUFF=sim.o logging.o utils.o elfloader.o osd.o

all: gdbserver musasim genheader dist
	
gdbserver: musashi $(BASESTUFF) hardware gdbserver.o argsgdb.o profiler.o
	$(CC) $(LFLAGS) -D GDBSERVER musashi/musashigdb.o gdbserver.o argsgdb.o profiler.o $(BASESTUFF) $(LIBCARDSGDB) $(LIBMODULES) ./hardware/board.o -o gdbserver -lelf

musasim: musashi $(BASESTUFF) hardware musasim.o args.o 
	$(CC) $(LFLAGS) musasim.o musashi/musashi.o args.o $(BASESTUFF) $(LIBCARDS) $(LIBMODULES) ./hardware/board.o -o $@ -lelf

musasim.o: musasim.c sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) musasim.c -o musasim.o

gdbserver.o: gdbserver.c gdbserver.h sim.h musashi/m68k.h musashi/m68kconf.h 
	$(CC) $(CFLAGS) $(LOCALCFLAGS) gdbserver.c -o gdbserver.o

sim.o: sim.c sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) sim.c -o sim.o

logging.o: logging.c logging.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) logging.c -o $@

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

osd.o: osd.c osd.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

args.o: args.h args.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) args.c -o $@

argsgdb.o: args.h args.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -D GDBSERVER args.c -o argsgdb.o

elfloader.o: elfloader.c elfloader.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<
profiler.o: profiler.c profiler.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

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
