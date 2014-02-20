include common.mk

LIBUI=./ui/libui.a
LIBCARDS=./hardware/cards/libcards.a
LIBCARDSGDB=./hardware/cards/libcardsgdb.a
LIBMODULES=./hardware/modules/libmodules.a
LOCALCFLAGS=--std=gnu99 `pkg-config --cflags $(ALLEXTLIBS)` -D_GNU_SOURCE=1

LOCALOBJECTS=sim.o logging.o utils.o elfloader.o throttler.o
GDBONLYOBJECTS=gdbserver.o argsgdb.o profiler.o
MUSASIMONLYOBJECTS=args.o
REMOTEOBJECTS=./hardware/registerplanner.o

all: checklibs gdbserver musasim genheader dist
	
gdbserver: musashi $(GDBONLYOBJECTS) $(LOCALOBJECTS) gdbserver.o ui hardware musashi
	$(CC) $(LFLAGS) -D GDBSERVER musashi/musashigdb.o $(LOCALOBJECTS) $(REMOTEOBJECTS) $(GDBONLYOBJECTS) $(LIBCARDSGDB) $(LIBMODULES)  $(LIBUI) ./hardware/boardgdb.o -o gdbserver -lelf

musasim: libmusasim.a musasim.o
	$(CC) $(LFLAGS) musasim.o libmusasim.a $(LIBCARDS) $(LIBMODULES) $(LIBUI) libmusasim.a -o $@ -lelf

libmusasim.a: musashi $(LOCALOBJECTS) $(MUSASIMONLYOBJECTS) ui hardware args.o musashi
	- rm $@
	ar cr $@ $(LOCALOBJECTS) args.o $(REMOTEOBJECTS) ./musashi/musashi.o ./hardware/board.o 

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

args.o: args.h args.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) args.c -o $@

argsgdb.o: args.h args.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -D GDBSERVER args.c -o argsgdb.o

elfloader.o: elfloader.c elfloader.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

profiler.o: profiler.c profiler.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

throttler.o: throttler.c throttler.h config.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

.PHONY: docs genheader musashi ui hardware clean checkcode checklibs

ui:
	$(MAKE) -C ui

hardware:
	$(MAKE) -C hardware

musashi:
	$(MAKE) -C musashi

genheader:
	$(MAKE) -C genheader

docs:
	rm -rf ./docs/
	mkdir ./docs/
	doxygen DoxyFile

dist:	musasim gdbserver
	mkdir dist
	cp musasim gdbserver dist/
	strip dist/musasim
	strip dist/gdbserver
	tar czf musasim.tar.gz dist/
	rm -r dist

clean:
	rm -f gdbserver musasim *.o *.a
	$(MAKE) -C musashi clean
	$(MAKE) -C genheader clean
	$(MAKE) -C hardware clean
	$(MAKE) -C ui clean
	
checkcode:
	cppcheck ./ --std=c99

checklibs:
	pkg-config --print-errors --exists $(ALLEXTLIBS)
	pkg-config --print-errors --atleast-version=$(PKGCONFIG_GLIB_MINVERSION) $(PKGCONFIG_GLIB)
