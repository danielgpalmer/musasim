#!/bin/bash
make clean
make
../../musasim/gdbserver -e prog.elf -c cfimage.bin --loguartchanone --loglevel=2 -p1234 $@
