ARGTABLE="argtable2"

all: png2bebin bin2c mkfontrom test 

bin2c: bin2c.c
	gcc -o bin2c bin2c.c	

png2bebin: png2bebin.c stb_image.o lzfx.o
	gcc -ggdb -std=gnu99 -o png2bebin png2bebin.c stb_image.o lzfx.o `pkg-config --cflags $(ARGTABLE)` `pkg-config --libs $(ARGTABLE)`

mkfontrom: mkfontrom.c
	gcc -std=gnu99 -ggdb -o mkfontrom mkfontrom.c	 `pkg-config --cflags $(ARGTABLE) freetype2` `pkg-config --libs freetype2 $(ARGTABLE)`

stb_image.o: stb_image.c
	gcc -D STBI_NO_HDR -c stb_image.c

lzfx.o: lzfx/lzfx.c lzfx/lzfx.h
	gcc -c $<

.PHONY: test clean

test: png2bebin
	./png2bebin -i test.png -o test.be16
	./png2bebin -i test.png -o test.bz -c
	./png2bebin -i test-alpha.png -o test-alpha.bz -c -m test-alpha.m
	#./mkfontrom -w8 -h16 -f /usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf -o test.fontrom.def
	#./mkfontrom -w8 -h16 -f /usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf -o test.fontrom.ascii -c ascii
	#./mkfontrom -w8 -h16 -f /usr/share/fonts/truetype/sazanami/sazanami-mincho.ttf -o test.fontrom.kana -c kana

	
clean:
	rm -f *.o *.be16 *.bz *.m png2bebin mkfontrom
