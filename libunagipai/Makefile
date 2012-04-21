ROOT_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
AR=m68k-elf-ar
CC=m68k-elf-gcc
CFLAGS=-mc68000 -ggdb -std=gnu99 -O2 -I ./include

INSTPATH="../toolchains/inst/m68k-elf/m68k-elf"

all: libunagipai.a docs

libunagipai.a: genheader ata.o dma.o uart.o machine.o video.o pff.o diskio.o utils.o lzfx.o image.o sprite.o math.o
	ar cr libunagipai.a *.o

uart.o: uart.c include/uart.h
	$(CC) $(CFLAGS) -c uart.c

machine.o: machine.c include/machine.h
	$(CC) $(CFLAGS) -c machine.c

video.o: video.c include/video.h
	$(CC) $(CFLAGS) -c video.c

ata.o: ata.c include/ata.h
	$(CC) $(CFLAGS) -c $<

dma.o: dma.c include/dma.h
	$(CC) $(CFLAGS) -c $<

pff.o: petitfatfs/pff.c
	$(CC) $(CFLAGS) -c $<

lzfx.o: lzfx/lzfx.c
	$(CC) $(CFLAGS) -O3 -c $<

image.o: image.c
	$(CC) $(CFLAGS) -c $<

diskio.o: petitfatfs/diskio.c
	$(CC) $(CFLAGS) -c $<

sprite.o: sprite.c
	$(CC) $(CFLAGS) -O3 -c $<

math.o: math.c include/math.h
	$(CC) $(CFLAGS) -O3 -c $<

utils.o: utils.c include/utils.h
	$(CC) $(CFLAGS) -c $<

.PHONY: docs genheader install uninstall clean

docs: 
	rm -rf docs
	mkdir docs
	doxygen DoxyFile

genheader:
	$(MAKE) INSTALLDIR="$(ROOT_DIR)/include" -C ../musasim/genheader/ install

install: libunagipai.a
	cp libunagipai.a $(INSTPATH)/lib/m68000/
	rm -rf $(INSTPATH)/include/libunagipai
	cp -rL include $(INSTPATH)/include/libunagipai

uninstall:
	rm -f $(INSTPATH)/lib/m68000/libunagipai.a
	rm -rf $(INSTPATH)/include/libunagipai
clean:
	rm -f *.a *.o
