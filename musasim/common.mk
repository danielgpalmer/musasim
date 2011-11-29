CC =     gcc
WARNINGS = -Wall -Werror
CFLAGS = $(WARNINGS) -O2 -finline-functions -c -Iobj -I. `pkg-config --cflags glib-2.0` `pkg-config --cflags gthread-2.0` `pkg-config --cflags sdl` -g
LFLAGS = $(WARNINGS) `pkg-config --libs glib-2.0` `pkg-config --libs gthread-2.0` `pkg-config --libs sdl`

