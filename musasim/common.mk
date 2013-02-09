#I don't want implicit targets 
.SUFFIXES:

MUSASIM_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
CC = gcc
WARNINGS = -Wall -Werror -Wstrict-aliasing # -Winline 

#you can override OPT to supply your own optimization flags
OPT ?= -O3 -march=native

PKGCONFIG_SDL = sdl
PKGCONFIG_GLIB = glib-2.0 
PKGCONFIG_GTHREAD = gthread-2.0

ALLEXTLIBS = $(PKGCONFIG_GLIB) $(PKGCONFIG_GTHREAD) argtable2 $(PKGCONFIG_SDL) SDL_ttf SDL_gfx fontconfig

LIBFLAGS=`pkg-config --libs $(ALLEXTLIBS)` -lrt

CFLAGS_GLIB = `pkg-config --cflags $(PKGCONFIG_GLIB)` -DG_DISABLE_ASSERT
CFLAGS_GTHREAD = `pkg-config --cflags $(PKGCONFIG_GTHREAD)`
CFLAGS_SDL = `pkg-config --cflags $(PKGCONFIG_SDL)`

INLININGOPTS= #--param inline-unit-growth=100 -finline-limit=1200 --param large-function-growth=5400
CFLAGS = $(WARNINGS) $(OPT) -c -g -I$(MUSASIM_DIR)/../libunagipai/include/ -ftree-vectorizer-verbose=2 $(INLININGOPTS)
LFLAGS = $(WARNINGS) $(OPT) $(LIBFLAGS) -lrt -flto $(INLININGOPTS)
