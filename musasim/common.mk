CC =     gcc
WARNINGS = -Wall -Werror
OPT = -O3

ALLEXTLIBS = glib-2.0 gthread-2.0 argtable2 sdl
SDL = sdl
CFLAGS = $(WARNINGS) $(OPT) -c -g
LFLAGS = $(WARNINGS) $(OPT) `pkg-config --libs $(ALLEXTLIBS)`

