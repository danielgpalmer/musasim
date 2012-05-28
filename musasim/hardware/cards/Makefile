include ../../common.mk

LOCALCFLAGS=--std=gnu99 `pkg-config --cflags $(SDL)` `pkg-config --cflags glib-2.0`

all: libcards.a libcardsgdb.a

libcardsgdb.a: romcardgdb.o uartcard.o videocard.o soundcard.o compactflashinterfacecard.o inputcard.o dmacard.o basicvideo.o basicsound.o blip_buf.o
	ar cr libcardsgdb.a $^

libcards.a: romcard.o uartcard.o videocard.o soundcard.o compactflashinterfacecard.o inputcard.o dmacard.o basicvideo.o basicsound.o blip_buf.o
	ar cr libcards.a $^

romcard.o: romcard.c romcard.h card.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

romcardgdb.o: romcard.c romcard.h card.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -DGDBSERVER -c $< -o $@

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

basicvideo.o: basicvideo.c basicvideo.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

basicsound.o: basicsound.c basicsound.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

blip_buf.o: blipbuf/blip_buf.c blipbuf/blip_buf.h
	gcc $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o *.a
