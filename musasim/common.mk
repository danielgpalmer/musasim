CC =     gcc
WARNINGS = -Wall -Werror
OPT = -O3
PACKAGES = glib-2.0 gthread-2.0 argtable2 sdl
CFLAGS = $(WARNINGS) $(OPT) -flto -c -Iobj -I. `pkg-config --cflags $(PACKAGES)` -g
LFLAGS = $(WARNINGS) -flto `pkg-config --libs $(PACKAGES)`

