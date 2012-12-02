TCPREFIX=../../toolchains/inst/m68k-elf/bin
TOOLSPREFIX=../../tools
CC=$(TCPREFIX)/m68k-elf-gcc 
AS=$(TCPREFIX)/m68k-elf-as
CP=$(TCPREFIX)/m68k-elf-objcopy

SOXBIGENDIANCONV=-tu16 -c 1 --endian big

