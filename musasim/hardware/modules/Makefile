include ../../common.mk

LOCALCFLAGS=-std=gnu99

all: libmodules.a

libmodules.a: timer.o bigtimer.o rtc.o
	ar cr $@ $^

timer.o: timer.c module.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

bigtimer.o: timer.c module.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) -DTIMER_BIGTIMER $< -o $@

rtc.o: rtc.c module.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

.PHONY: clean
	
clean:
	rm -f *.o *.a
