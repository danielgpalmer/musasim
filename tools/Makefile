ARGTABLE="argtable2"

all: png2bebin bin2c mkfontrom test 

bin2c: bin2c.c
	gcc -o bin2c bin2c.c	

png2bebin: png2bebin.c stb_image.o
	gcc -std=gnu99 -o png2bebin png2bebin.c stb_image.o `pkg-config --cflags $(ARGTABLE)` `pkg-config --libs $(ARGTABLE)`

mkfontrom: mkfontrom.c
	gcc -std=gnu99 -ggdb -o mkfontrom mkfontrom.c	 `pkg-config --cflags $(ARGTABLE) freetype2` `pkg-config --libs freetype2 $(ARGTABLE)`

stb_image.o: stb_image.c
	gcc -D STBI_NO_HDR -c stb_image.c

.PHONY: test clean

test:
	./png2bebin -i test.png -o test.be16
	./mkfontrom -w8 -h16 -f /usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf -o test.fontrom
	
clean:
	rm *.o
	rm test.be16
	rm test.fontrom
	rm png2bebin
	rm mkfontrom
