include ./common.mk

AFLAGS=-ahls -m68000
CFLAGS=-mc68000 -ggdb -std=gnu99 -Wall -O2 -Wstrict-aliasing
LFLAGS=-mc68000 -Trom.lds -nostartfiles -Wl,-u_write_r,-u_close_r,-u_isatty_r,-u_lseek_r,-u_sbrk_r,-u_fstat_r,-u_read_r,-u_impure_ptr,-u_global_impure_ptr,-Map=prog.map,--cref

all: prog.bin prog.srec cfimage.bin

prog.bin: prog.elf
	$(CP) -O binary prog.elf prog.bin

prog.srec: prog.elf
	$(CP) -S -O srec prog.elf prog.srec
	
prog.elf: main.o crt.o rom.lds
	$(CC) $(LFLAGS) -o prog.elf crt.o main.o -lunagipai

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c --save-temps

crt.o: crt.s
	$(AS) $(AFLAGS) crt.s -o crt.o > crt.lst

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.srec
	rm -f *.map
	rm -f *.elf
	rm -f *.lst

.PHONY: cfimage.bin

cfimage.bin: cfimage/kick1.as2 cfimage/sine.as2
	dd if=/dev/zero of=cfimage.bin bs=1MB count=64
	/sbin/mkfs.vfat cfimage.bin
	MTOOLSRC=./mtools.conf mcopy cfimage/kick1.as2 m:/
	MTOOLSRC=./mtools.conf mcopy cfimage/sine.as2 m:/
	MTOOLSRC=./mtools.conf mdir m:	

cfimage/kick1.as2: samples/kick1.wav
	sox $< $(SOXBIGENDIANCONV) $@	

cfimage/sine.as2: sine.wav
	sox $< $(SOXBIGENDIANCONV) $@
