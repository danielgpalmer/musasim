AR=m68k-elf-ar
CC=m68k-elf-gcc
CFLAGS=-mc68000 -ggdb -std=gnu99 -I ./include

all: libunagipai.a

libunagipai.a: uart.o
	ar cr libunagipai.a uart.o

uart.o: uart.c include/uart.h
	$(CC) $(CFLAGS) -c uart.c

.PHONY: install clean

install: libunagipai.a
	cp libunagipai.a ../toolchain/inst/m68k-elf/m68k-elf/lib/m68000/
	rm -rf ../toolchain/inst/m68k-elf/m68k-elf/include/libunagipai
	cp -r include ../toolchain/inst/m68k-elf/m68k-elf/include/libunagipai

clean:
	rm *.a *.o
