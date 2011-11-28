include ../../common.mk

libcards.a: romcard.o uartcard.o videocard.o soundcard.o
	ar cr libcards.a romcard.o uartcard.o videocard.o soundcard.o	

romcard.o: romcard.c romcard.h
	gcc $(CFLAGS) -c romcard.c -o romcard.o

uartcard.o: uartcard.c uartcard.h
	gcc $(CFLAGS) -c uartcard.c -o uartcard.o	

videocard.o: videocard.c videocard.h
	gcc $(CFLAGS) -c videocard.c -o videocard.o

soundcard.o: soundcard.c soundcard.h
	gcc $(CFLAGS) -c soundcard.c -o soundcard.o

clean:
	rm -f *.o *.a
