#!/bin/bash
make clean
make
../../musasim/musasim -e prog.elf -c cfimage.bin --loguartchanone --loglevel=2
