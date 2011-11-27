include ../../common.mk

libcards.a: romcard.o uartcard.o videocard.o
	ar cr libcards.a romcard.o uartcard.o videocard.o	

romcard.o: romcard.c romcard.h
	gcc $(CFLAGS) -c romcard.c -o romcard.o

uartcard.o: uartcard.c uartcard.h
	gcc $(CFLAGS) -c uartcard.c -o uartcard.o	

videocard.o: videocard.c videocard.h
	gcc $(CFLAGS) -c videocard.c -o videocard.o

clean:
	rm -f *.o *.a
