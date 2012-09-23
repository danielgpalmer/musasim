include ../../common.mk

LOCALCFLAGS=-std=gnu99

all: libmodules.a

libmodules.a: timer.o rtc.o
	ar cr $@ $^

timer.o: timer.c module.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) 4< -o $@

rtc.o: rtc.c module.h
	$(CC) $(CFLAGS) $(LOCALCFLAGS) $< -o $@

.PHONY: clean
	
clean:
	rm -f *.o *.a
