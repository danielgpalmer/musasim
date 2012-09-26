TCPREFIX=../../toolchains/inst/m68k-elf/bin
CC=$(TCPREFIX)/m68k-elf-gcc 
AS=$(TCPREFIX)/m68k-elf-as
CP=$(TCPREFIX)/m68k-elf-objcopy

AFLAGS=-ahls -m68000
CFLAGS=-mc68000 -ggdb -std=gnu99 -Wall -O2 -Wstrict-aliasing
LFLAGS=-mc68000 -Trom.lds -nostartfiles -Wl,-Map=prog.map,--cref

all: prog.bin prog.srec

prog.bin: prog.elf
	$(CP) -O binary prog.elf prog.bin

prog.srec: prog.elf
	$(CP) -S -O srec prog.elf prog.srec
	
prog.elf: icklebickle.o rom.lds
	$(CC) $(LFLAGS) -o prog.elf icklebickle.o

icklebickle.o: icklebickle.s
	$(AS) $(AFLAGS) $< -o $@ > crt.lst

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.srec
	rm -f *.map
	rm -f *.elf
	rm -f *.lst
