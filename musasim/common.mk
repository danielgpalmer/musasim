#I don't want implicit targets 
.SUFFIXES:

MUSASIM_DIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

#you can override OPT to supply your own optimization flags

ifeq ($(shell uname -s),Darwin)
CC = clang
WARNINGS = -Wall -Wstrict-aliasing -D_DARWIN_C_SOURCE
INLININGOPTS= #--param inline-unit-growth=100 -finline-limit=1200 --param large-function-growth=5400
OPT ?= -O3
OS_LIBS =
OS_PKGCONFIG = libelf
else
CC = gcc
WARNINGS = -Wall -Werror -Wstrict-aliasing # -Winline 
INLININGOPTS= #--param inline-unit-growth=100 -finline-limit=1200 --param large-function-growth=5400
OPT ?= -O3 -march=native -ftree-vectorizer-verbose=2 
OS_LIBS = -lrt -flto
OS_PKGCONFIG = 
endif


PKGCONFIG_GLIB = glib-2.0 
PKGCONFIG_GLIB_MINVERSION = 2.32.0
PKGCONFIG_GTHREAD = gthread-2.0
PKGCONFIG_SDL2TTF = SDL2_ttf

ALLEXTLIBS = $(PKGCONFIG_GLIB) $(PKGCONFIG_GTHREAD) argtable2 $(PKGCONFIG_SDL) $(PKGCONFIG_SDL2TTF) $(OS_PKGCONFIG) fontconfig

LIBFLAGS=`pkg-config --libs $(ALLEXTLIBS)` $(OS_LIBS)

CFLAGS_GLIB = `pkg-config --cflags $(PKGCONFIG_GLIB)` -DG_DISABLE_ASSERT
CFLAGS_GTHREAD = `pkg-config --cflags $(PKGCONFIG_GTHREAD)`

CFLAGS = $(WARNINGS) $(OPT) -c -g -I$(MUSASIM_DIR)/../libunagipai/include/ $(INLININGOPTS)
LFLAGS = $(WARNINGS) $(OPT) $(LIBFLAGS) $(INLININGOPTS)
