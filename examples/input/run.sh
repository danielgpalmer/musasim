#!/bin/bash
make clean
make
../../musasim/musasim -e prog.elf --loguartchanone --disablesound --loglevel=2 
