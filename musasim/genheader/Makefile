include ../common.mk

all: machine_stuff.h dma_registers.h ata_registers.h video_registers.h sound_registers.h input_registers.h uart_registers.h timers_registers.h

machine_stuff.h: genheader
	./genheader machine > $@
	$(CC) -x c -fsyntax-only $@

dma_registers.h: genheader
	./genheader dma > dma_registers.h
	$(CC) -x c -fsyntax-only dma_registers.h

ata_registers.h: genheader
	./genheader ata > ata_registers.h

video_registers.h: genheader
	./genheader video > video_registers.h
	$(CC) -x c -fsyntax-only video_registers.h

sound_registers.h: genheader
	./genheader sound > sound_registers.h
	$(CC) -x c -fsyntax-only sound_registers.h

input_registers.h: genheader
	./genheader input > input_registers.h
	$(CC) -x c -fsyntax-only input_registers.h
	
uart_registers.h: genheader
	./genheader uart > uart_registers.h
	$(CC) -x c -fsyntax-only uart_registers.h

timers_registers.h: genheader
	./genheader timers > $@
	$(CC) -x c -fsyntax-only $@

genheader: genheader.c Makefile ../utils.c ../hardware/cards/dmacard.h
	$(CC) -std=gnu99 $(CFLAGS_GLIB) genheader.c ../utils.c   ../hardware/cards/libcards.a ../hardware/modules/libmodules.a ../libmusasim.a ../ui/libui.a $(LIBFLAGS) -lelf -o $@

.PHONY: install clean

install: all
	echo "installing headers to $(INSTALLDIR)"
	cp *.h $(INSTALLDIR)

clean:
	rm -f genheader *.h
