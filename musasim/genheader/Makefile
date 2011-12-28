all: machine.h video.h sound.h input.h uart.h

machine.h: genheader ../hardware/cards/dmaregisters.h ../hardware/cards/atacommands.h
	./genheader machine > machine.h
	cat ../hardware/cards/atacommands.h >> machine.h
	cat ../hardware/cards/dmaregisters.h >> machine.h
	gcc -x c -fsyntax-only machine.h

video.h: genheader ../hardware/cards/videoregistermasks.h
	./genheader video > video.h
	cat ../hardware/cards/videoregistermasks.h >> video.h
	gcc -x c -fsyntax-only video.h

sound.h: genheader
	./genheader sound > sound.h
	gcc -x c -fsyntax-only sound.h

input.h: genheader
	./genheader input > input.h
	gcc -x c -fsyntax-only input.h
	
uart.h: genheader ../hardware/cards/uartregistermasks.h
	./genheader uart > uart.h
	cat ../hardware/cards/uartregistermasks.h >> uart.h
	gcc -x c -fsyntax-only uart.h

genheader: genheader.c Makefile ../utils.c ../hardware/cards/dmacard.h
	gcc genheader.c ../utils.c -o genheader

clean:
	rm -f genheader video.h machine.h sound.h input.h uart.h
