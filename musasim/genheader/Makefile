all: machine.h video.h sound.h

machine.h: genheader
	./genheader machine > machine.h
	gcc -x c -fsyntax-only machine.h

video.h: genheader
	./genheader video > video.h
	gcc -x c -fsyntax-only machine.h

sound.h: genheader
	./genheader sound > sound.h
	gcc -x c -fsyntax-only machine.h

genheader: genheader.c
	gcc genheader.c -o genheader

clean:
	rm -f genheader video.h machine.h sound.h
