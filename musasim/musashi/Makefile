m68kcpu.o: ../sim.h m68kops.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) m68kcpu.c -o m68kcpu.o

m68kcpugdb.o: ../sim.h m68kops.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) -D GDBSERVER m68kcpu.c -o m68kcpugdb.o

m68kops.o: m68kmake m68kops.h m68kops.c ../sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) m68kops.c -o m68kops.o

m68kopac.o: m68kmake m68kops.h m68kopac.c ../sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) m68kopac.c -o m68kopac.o

m68kopdm.o: m68kmake m68kops.h m68kopdm.c ../sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) m68kopdm.c -o m68kopdm.o

m68kopnz.o: m68kmake m68kops.h m68kopnz.c ../sim.h m68k.h m68kconf.h
	$(CC) $(CFLAGS) m68kopnz.c -o m68kopnz.o

m68kops.h: m68kmake
	./m68kmake ./ m68k_in.c

m68kmake: m68kmake.c m68k_in.c
	$(CC) $(WARNINGS) m68kmake.c -o m68kmake

# these are made by the called to m68kmake for m68kops.h
m68kops.c: m68kops.h

m68kopac.c: m68kops.h

m68kopdm.c: m68kops.h

m68kopnz.c: m68kops.h
#

clean:
	rm -f m68kmake m68kops.c m68kopac.c m68kopdm.c m68kopnz.c m68kops.h
