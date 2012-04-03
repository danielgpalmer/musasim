CC =     gcc
WARNINGS = -Wall -Werror -Wstrict-aliasing
OPT = -O3 -march=native
ALLEXTLIBS = glib-2.0 argtable2 sdl
SDL = sdl
CFLAGS = $(WARNINGS) $(OPT) -c -g
LFLAGS = $(WARNINGS) $(OPT) `pkg-config --libs $(ALLEXTLIBS)`

