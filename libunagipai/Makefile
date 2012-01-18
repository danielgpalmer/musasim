AR=m68k-elf-ar
CC=m68k-elf-gcc
CFLAGS=-mc68000 -ggdb -std=gnu99

all: libunagipai.a

libunagipai.a: uart.o
	ar cr libunagipai.a uart.o

uart.o: uart.c uart.h
	$(CC) $(CFLAGS) -c uart.c

.PHONY: install

install: libunagipai.a
	cp libunagipai.a ../toolchain/inst/m68k-elf/m68k-elf/lib/
