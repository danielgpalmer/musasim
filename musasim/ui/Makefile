include ../common.mk
include ../sdl.mk

LOCALCFLAGS=--std=gnu99 -I../ $(CFLAGS_SDL2) $(CFLAGS_GLIB)

all: libui.a

libui.a: renderer.o osd.o sdlwrapper.o fontutils.o input.o
	rm -f $@
	ar cr libui.a $^

osd.o: osd.c osd.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

sdlwrapper.o: sdlwrapper.c sdlwrapper.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

fontutils.o: fontutils.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

renderer.o: renderer.c renderer.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

input.o: input.c input.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -o $@ $<

.PHONY: clean

clean:
	rm -f *.o *.a
