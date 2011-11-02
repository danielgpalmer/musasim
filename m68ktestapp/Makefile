PKG_CONFIG_LIBDIR=/home/daniel/toolchains/inst/m68k-elf/lib/ 
PKG_CONFIG_PATH=/home/daniel/toolchains/inst/m68k-elf/lib/pkgconfig

CC=m68k-elf-gcc 
AS=m68k-elf-as
CP=m68k-elf-objcopy

AFLAGS=-ahls
CFLAGS=-mc68000 -ggdb -std=gnu99
LFLAGS=-Trom.lds -nostartfiles  -Wl,-Map=prog.map

all: prog.bin prog.srec

prog.bin: prog.elf
	$(CP) -O binary prog.elf prog.bin

prog.srec: prog.elf
	$(CP) -O srec prog.elf prog.srec
	
prog.elf: crt.o main.o fontrom.o rom.lds
	$(CC) $(LFLAGS) -o prog.elf crt.o main.o fontrom.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

crt.o: crt.s
	$(AS) $(AFLAGS) crt.s -o crt.o > crt.lst

fontrom.o: fontrom/fontrom.c
	$(CC) $(CFLAGS) -c fontrom/fontrom.c
clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.srec
	rm -f *.map
	rm -f *.elf
	rm -f *.lst
