#I don't want implicit targets 
.SUFFIXES:

MUSASIM_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
CC =     gcc
WARNINGS = -Wall -Werror -Wstrict-aliasing #-Winline
OPT = -O3 -march=native
ALLEXTLIBS = glib-2.0 argtable2 sdl SDL_ttf SDL_gfx fontconfig
LIBFLAGS=`pkg-config --libs $(ALLEXTLIBS)` -lrt
SDL = sdl
CFLAGS = $(WARNINGS) $(OPT) -c -g -I$(MUSASIM_DIR)/../libunagipai/include/ -mfpmath=sse -flto -ftree-vectorizer-verbose=2
LFLAGS = $(WARNINGS) $(OPT) $(LIBFLAGS) -lrt -flto --param inline-unit-growth=100 -finline-limit=1200 --param large-function-growth=5400

