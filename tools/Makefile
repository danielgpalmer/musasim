all: png2bebin test 

png2bebin: png2bebin.c stb_image.o
	gcc -o png2bebin png2bebin.c stb_image.o `pkg-config --cflags argtable2` `pkg-config --libs argtable2`

stb_image.o: stb_image.c
	gcc -D STBI_NO_HDR -c stb_image.c



.PHONY: test clean

test:
	./png2bebin -i test.png -o test.be16
	

clean:
	rm *.o
	rm png2bebin
