include ../../common.mk
include ../../sdl.mk

LOCALCFLAGS=--std=gnu99
DEPS=card.h ../board.h

all: libcards.a libcardsgdb.a

libcardsgdb.a: romcardgdb.o uartcard.o videocard.o soundcard.o compactflashinterfacecard.o inputcard.o dmacard.o basicvideo.o basicsound.o blip_buf.o timercard.o ringbuffer.o
	rm -f $@
	ar cr libcardsgdb.a $^

libcards.a: romcard.o uartcard.o videocard.o soundcard.o compactflashinterfacecard.o inputcard.o dmacard.o basicvideo.o basicsound.o blip_buf.o timercard.o ringbuffer.o
	rm -f $@
	ar cr libcards.a $^

romcard.o: romcard.c romcard.h $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

romcardgdb.o: romcard.c romcard.h $(DEPS) ../../gdbserver.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -DGDBSERVER -c $< -o $@

#glib needed for datastructures
uartcard.o: uartcard.c uartcard.h $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $(CFLAGS_GLIB) -c uartcard.c -o uartcard.o	

#sdl needed for video
#glib needed for G_N_ELEMENTS
videocard.o: videocard.c videocard.h $(DEPS) ../../sim.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $(CFLAGS_GLIB) $(CFLAGS_SDL2) -c videocard.c -o videocard.o

#sdl needed for sound
soundcard.o: soundcard.c soundcard.h ringbuffer.o $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $(CFLAGS_SDL2) -c soundcard.c -o soundcard.o

#glib needed for endian converion macro
compactflashinterfacecard.o: compactflashinterfacecard.c compactflashinterfacecard.h $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $(CFLAGS_GLIB) -c compactflashinterfacecard.c -o compactflashinterfacecard.o

inputcard.o: inputcard.c inputcard.h $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $(CFLAGS_SDL2) -c inputcard.c -o inputcard.o

dmacard.o: dmacard.c dmacard.h card.h $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -c dmacard.c -o dmacard.o

basicvideo.o: basicvideo.c basicvideo.h $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

basicsound.o: basicsound.c basicsound.h $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $(CFLAGS_SDL2) -c $< -o $@

timercard.o: timercard.c timercard.h $(DEPS)
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

blip_buf.o: blipbuf/blip_buf.c blipbuf/blip_buf.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

ringbuffer.o: ringbuffer.c ringbuffer.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o *.a
