include ../../common.mk

LOCALCFLAGS=-std=gnu99

all: libmodules.a

libmodules.a:	timer.o
	ar cr $@ $^

timer.o: timer.c
	$(CC) $(CFLAGS) $(LOCALCFLAGS) timer.c -o timer.o

.PHONY: clean
	
clean:
	rm -f *.o *.a
