AR=m68k-elf-ar
CC=m68k-elf-gcc
CFLAGS=-mc68000 -ggdb -std=gnu99 -I ./include

INSTPATH="../toolchain/inst/m68k-elf/m68k-elf"

all: libunagipai.a

libunagipai.a: uart.o machine.o video.o
	ar cr libunagipai.a *.o

uart.o: uart.c include/uart.h
	$(CC) $(CFLAGS) -c uart.c

machine.o: machine.c include/machine.h
	$(CC) $(CFLAGS) -c machine.c

video.o: video.c include/video.h
	$(CC) $(CFLAGS) -c video.c

.PHONY: install uninstall clean

install: libunagipai.a
	cp libunagipai.a $(INSTPATH)/lib/m68000/
	rm -rf $(INSTPATH)/include/libunagipai
	cp -r include $(INSTPATH)/include/libunagipai

uninstall:
	rm -f $(INSTPATH)/lib/m68000/libunagipai.a
	rm -rf $(INSTPATH)/include/libunagipai
clean:
	rm *.a *.o
