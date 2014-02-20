#!/bin/bash
make clean
make
../../musasim/gdbserver -e prog.elf --loguartchanone --loglevel=2 -p1234 $@
