PKG_CONFIG_LIBDIR=/home/daniel/toolchains/inst/m68k-elf/lib/ 
PKG_CONFIG_PATH=/home/daniel/toolchains/inst/m68k-elf/lib/pkgconfig

CC=m68k-elf-gcc 
AS=m68k-elf-as
CP=m68k-elf-objcopy

AFLAGS=-ahls
CFLAGS=-mc68000 -ggdb -std=gnu99
LFLAGS=-Trom.lds -nostartfiles  -Wl,-Map=prog.map

all: prog.bin prog.srec cfimage.bin

prog.bin: prog.elf
	$(CP) -O binary prog.elf prog.bin

prog.srec: prog.elf
	$(CP) -O srec prog.elf prog.srec
	
prog.elf: syscalls.o crt.o main.o fontrom.o rom.lds
	$(CC) $(LFLAGS) -o prog.elf crt.o syscalls.o main.o fontrom.o

main.o: blip.c main.c ../musasim/genheader/machine.h ../musasim/genheader/video.h ../musasim/genheader/input.h ../musasim/genheader/uart.h
	$(CC) $(CFLAGS) -c main.c --save-temps

syscalls.o: syscalls.c
	$(CC) $(CFLAGS) -c syscalls.c --save-temps

crt.o: crt.s
	$(AS) $(AFLAGS) crt.s -o crt.o > crt.lst

fontrom.o: fontrom/fontrom.c
	$(CC) $(CFLAGS) -c fontrom/fontrom.c

cfimage.bin:
	dd if=/dev/zero of=cfimage.bin bs=1MB count=64
	/sbin/mkfs.vfat cfimage.bin

blip.c: blip.be16
	../tools/bin2c blip.be16 blip blip

blip.be16: blip.wav
	 sox blip.wav -t raw -b 16 --endian big blip.be16

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.srec
	rm -f *.map
	rm -f *.elf
	rm -f *.lst
