TCPREFIX=../../toolchains/inst/m68k-elf/bin
TOOLSPREFIX=../../tools
CC=$(TCPREFIX)/m68k-elf-gcc 
AS=$(TCPREFIX)/m68k-elf-as
CP=$(TCPREFIX)/m68k-elf-objcopy

AFLAGS=-ahls -m68000
CFLAGS=-mc68000 -ggdb -std=gnu99 -Wall -O2 -Wstrict-aliasing
LFLAGS=-mc68000 -Trom.lds -nostartfiles -Wl,-u_write_r,-u_close_r,-u_isatty_r,-u_lseek_r,-u_sbrk_r,-u_fstat_r,-u_read_r,-u_impure_ptr,-u_global_impure_ptr,-Map=prog.map,--cref -flto

all: prog.bin prog.srec

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
	rm -f main.s main.i
