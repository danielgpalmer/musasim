all: machine.h video.h sound.h input.h uart.h

machine.h: genheader
	./genheader machine > machine.h
	gcc -x c -fsyntax-only machine.h

video.h: genheader
	./genheader video > video.h
	gcc -x c -fsyntax-only video.h

sound.h: genheader
	./genheader sound > sound.h
	gcc -x c -fsyntax-only sound.h

input.h: genheader
	./genheader input > input.h
	gcc -x c -fsyntax-only input.h
	
uart.h: genheader
	./genheader uart > uart.h
	gcc -x c -fsyntax-only uart.h

genheader: genheader.c Makefile
	gcc genheader.c -o genheader

clean:
	rm -f genheader video.h machine.h sound.h input.h uart.h
