#!/bin/sh

OPT="-O3 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=hard -funsafe-math-optimizations" make
