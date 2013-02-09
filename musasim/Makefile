include common.mk

LIBCARDS=./hardware/cards/libcards.a
LIBCARDSGDB=./hardware/cards/libcardsgdb.a
LIBMODULES=./hardware/modules/libmodules.a
LOCALCFLAGS=--std=gnu99 `pkg-config --cflags $(ALLEXTLIBS)`

LOCALOBJECTS=sim.o logging.o utils.o elfloader.o osd.o fontutils.o sdlwrapper.o
GDBONLYOBJECTS=gdbserver.o argsgdb.o profiler.o
MUSASIMONLYOBJECTS=args.o
REMOTEOBJECTS=./hardware/registerplanner.o

all: checklibs gdbserver musasim genheader dist
	
gdbserver: musashi $(GDBONLYOBJECTS) $(LOCALOBJECTS) gdbserver.o hardware musashi
	$(CC) $(LFLAGS) -D GDBSERVER musashi/musashigdb.o $(LOCALOBJECTS) $(REMOTEOBJECTS) $(GDBONLYOBJECTS) $(LIBCARDSGDB) $(LIBMODULES) ./hardware/boardgdb.o -o gdbserver -lelf

musasim: libmusasim.a
	$(CC) $(LFLAGS) libmusasim.a $(LIBCARDS) $(LIBMODULES) libmusasim.a -o $@ -lelf

libmusasim.a: musashi $(LOCALOBJECTS) $(MUSASIMONLYOBJECTS) hardware musasim.o args.o musashi
	- rm $@
	ar cr $@ $(LOCALOBJECTS) musasim.o args.o $(REMOTEOBJECTS) ./musashi/musashi.o ./hardware/board.o 

musasim.o: musasim.c sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) musasim.c -o musasim.o

gdbserver.o: gdbserver.c gdbserver.h sim.h hardware/board.h musashi/m68k.h musashi/m68kconf.h 
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

sim.o: sim.c sim.h musashi/m68k.h musashi/m68kconf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) sim.c -o sim.o

logging.o: logging.c logging.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) logging.c -o $@

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

osd.o: osd.c osd.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

sdlwrapper.o: sdlwrapper.c sdlwrapper.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

args.o: args.h args.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) args.c -o $@

argsgdb.o: args.h args.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -D GDBSERVER args.c -o argsgdb.o

elfloader.o: elfloader.c elfloader.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

profiler.o: profiler.c profiler.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

fontutils.o: fontutils.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

.PHONY: docs genheader musashi hardware clean checkcode checklibs

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
	rm -f *.a
	$(MAKE) -C musashi clean
	$(MAKE) -C genheader clean
	$(MAKE) -C hardware clean

checkcode:
	cppcheck ./ --std=c99

checklibs:
	pkg-config --exists $(ALLEXTLIBS)
