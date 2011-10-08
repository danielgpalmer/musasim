CC =     gcc
WARNINGS = -Wall -Werror
CFLAGS = $(WARNINGS) -O2 -finline-functions -c -Iobj -I. `pkg-config --cflags glib-2.0` `pkg-config --cflags gthread-2.0` `pkg-config --cflags sdl` -g
LFLAGS = $(WARNINGS) `pkg-config --libs glib-2.0` `pkg-config --libs gthread-2.0` `pkg-config --libs sdl`

all: obj gdbserver musasim

clean:
	rm -rf obj
	rm -f gdbserver
	rm -f musasim
obj:
	mkdir obj

gdbserver: obj/gdbserver.o obj/common.o obj/sim.o obj/m68kcpu.o obj/m68kops.o obj/m68kopac.o obj/m68kopdm.o obj/m68kopnz.o obj/video.o
	$(CC) $(LFLAGS) obj/gdbserver.o obj/common.o obj/sim.o obj/m68kcpu.o obj/m68kops.o obj/m68kopac.o obj/m68kopdm.o obj/m68kopnz.o obj/video.o -o gdbserver

musasim: obj/musasim.o obj/common.o obj/sim.o obj/m68kcpu.o obj/m68kops.o obj/m68kopac.o obj/m68kopdm.o obj/m68kopnz.o obj/video.o
	$(CC) $(LFLAGS) obj/musasim.o obj/common.o obj/sim.o obj/m68kcpu.o obj/m68kops.o obj/m68kopac.o obj/m68kopdm.o obj/m68kopnz.o obj/video.o -o musasim

obj/gdbserver.o: gdbserver.c gdbserver.h sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) --std=gnu99 gdbserver.c -o obj/gdbserver.o

obj/musasim.o: musasim.c sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) --std=gnu99 musasim.c -o obj/musasim.o

obj/common.o: common.c
	$(CC) $(CFLAGS) --std=gnu99 common.c -o obj/common.o

obj/sim.o: sim.c sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) --std=gnu99 sim.c -o obj/sim.o

obj/m68kcpu.o: sim.h obj/m68kops.h sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) m68kcpu.c -o obj/m68kcpu.o

obj/m68kops.o: obj/m68kmake obj/m68kops.h obj/m68kops.c sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) obj/m68kops.c -o obj/m68kops.o

obj/m68kopac.o: obj/m68kmake obj/m68kops.h obj/m68kopac.c sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) obj/m68kopac.c -o obj/m68kopac.o

obj/m68kopdm.o: obj/m68kmake obj/m68kops.h obj/m68kopdm.c sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) obj/m68kopdm.c -o obj/m68kopdm.o

obj/m68kopnz.o: obj/m68kmake obj/m68kops.h obj/m68kopnz.c sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) obj/m68kopnz.c -o obj/m68kopnz.o

obj/m68kops.h: obj/m68kmake
	obj/m68kmake obj m68k_in.c

obj/m68kmake: m68kmake.c m68k_in.c
	$(CC) $(WARNINGS) m68kmake.c -o obj/m68kmake

# hardware 

obj/video.o: hardware/video.c hardware/video.h
	$(CC) $(CFLAGS) --std=gnu99 hardware/video.c -o obj/video.o

