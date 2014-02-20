#!/bin/bash
make clean
make
valgrind --tool=callgrind ../../musasim/musasim -e prog.elf -c cfimage.bin --loguartchanone --loglevel=2
