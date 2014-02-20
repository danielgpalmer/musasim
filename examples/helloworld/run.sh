#!/bin/bash
make clean
make
../../musasim/musasim -e prog.elf --loguartchanone --loglevel=0 $@
