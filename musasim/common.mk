MUSASIM_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
CC =     gcc
WARNINGS = -Wall -Werror -Wstrict-aliasing
OPT = -O3 -march=native -fstack-protector-all
ALLEXTLIBS = glib-2.0 argtable2 sdl
SDL = sdl
CFLAGS = $(WARNINGS) $(OPT) -c -g -I$(MUSASIM_DIR)/../libunagipai/include/ -lmcheck
LFLAGS = $(WARNINGS) $(OPT) `pkg-config --libs $(ALLEXTLIBS)` -lrt

