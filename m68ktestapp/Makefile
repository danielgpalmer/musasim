TCPREFIX=../toolchains/inst/m68k-elf/bin
CC=$(TCPREFIX)/m68k-elf-gcc 
AS=$(TCPREFIX)/m68k-elf-as
CP=$(TCPREFIX)/m68k-elf-objcopy

AFLAGS=-ahls -m68000
CFLAGS=-mc68000 -ggdb -std=gnu99 -Wall -O2 -Wstrict-aliasing
LFLAGS=-mc68000 -Trom.lds -nostartfiles -Wl,-Map=prog.map,--cref,-lunagipai

all: prog.bin prog.srec cfimage.bin

prog.bin: prog.elf
	$(CP) -O binary prog.elf prog.bin

prog.srec: prog.elf
	$(CP) -S -O srec prog.elf prog.srec
	
prog.elf: fontrom.o crt.o syscalls.o main.o rom.lds
	$(CC) $(LFLAGS) -o prog.elf crt.o main.o fontrom.o syscalls.o -lunagipai

main.o: blip.c main.c
	$(CC) $(CFLAGS) -c main.c --save-temps

crt.o: crt.s
	$(AS) $(AFLAGS) crt.s -o crt.o > crt.lst

syscalls.o: syscalls.c
	$(CC) $(CFLAGS) -c syscalls.c


fontrom.o:
	../tools/mkfontrom -w8 -h16 -f /usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf  -o fontrom.bin
	../tools/bin2c fontrom.bin fontrom fontrom
	$(CC) $(CFLAGS) -c fontrom.c

cfimage.bin:
	dd if=/dev/zero of=cfimage.bin bs=1MB count=64
	/sbin/mkfs.vfat cfimage.bin
	../tools/png2bebin -i rawfiles/pai.png -o cfimagefiles/pai.bz -c
	../tools/png2bebin -i rawfiles/ball.png -o cfimagefiles/ball.be
	../tools/png2bebin -i rawfiles/check.png -o cfimagefiles/check.be
	MTOOLSRC=./mtools.conf mcopy cfimagefiles/pai.bz m:/	
	MTOOLSRC=./mtools.conf mcopy cfimagefiles/ball.be m:/
	MTOOLSRC=./mtools.conf mcopy cfimagefiles/check.be m:/
	MTOOLSRC=./mtools.conf mcopy cfimagefiles/test.txt m:/
	MTOOLSRC=./mtools.conf mdir m:

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
