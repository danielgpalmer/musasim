include ../../common.mk

LOCALCFLAGS=--std=gnu99

libcards.a: romcard.o uartcard.o videocard.o soundcard.o compactflashinterfacecard.o inputcard.o dmacard.o
	ar cr libcards.a romcard.o uartcard.o videocard.o soundcard.o compactflashinterfacecard.o inputcard.o dmacard.o

romcard.o: romcard.c romcard.h card.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c romcard.c -o romcard.o

uartcard.o: uartcard.c uartcard.h card.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c uartcard.c -o uartcard.o	

videocard.o: videocard.c videocard.h card.h ../../sim.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c videocard.c -o videocard.o

soundcard.o: soundcard.c soundcard.h card.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c soundcard.c -o soundcard.o

compactflashinterfacecard.o: compactflashinterfacecard.c compactflashinterfacecard.h card.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c compactflashinterfacecard.c -o compactflashinterfacecard.o

inputcard.o: inputcard.c inputcard.h card.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c inputcard.c -o inputcard.o

dmacard.o: dmacard.c dmacard.h card.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c dmacard.c -o dmacard.o


clean:
	rm -f *.o *.a
