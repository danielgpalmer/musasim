CC =     gcc
WARNINGS = -Wall -Werror
OPT = -O0
CFLAGS = $(WARNINGS) $(OPT) -c -Iobj -I. `pkg-config --cflags glib-2.0` `pkg-config --cflags gthread-2.0` `pkg-config --cflags sdl` `pkg-config --cflags argtable2` -g
LFLAGS = $(WARNINGS) `pkg-config --libs glib-2.0` `pkg-config --libs gthread-2.0` `pkg-config --libs sdl` `pkg-config --libs argtable2`

