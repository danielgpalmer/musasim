#I don't want implicit targets 
.SUFFIXES:

MUSASIM_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
CC =     gcc
WARNINGS = -Wall -Werror -Wstrict-aliasing
OPT = -O3 -march=native -fstack-protector-all
ALLEXTLIBS = glib-2.0 argtable2 sdl SDL_ttf SDL_gfx fontconfig
LIBFLAGS=`pkg-config --libs $(ALLEXTLIBS)` -lrt
SDL = sdl
CFLAGS = $(WARNINGS) $(OPT) -c -g -I$(MUSASIM_DIR)/../libunagipai/include/ -flto
LFLAGS = $(WARNINGS) $(OPT) $(LIBFLAGS) -lrt -flto

