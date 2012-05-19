TOP=$(shell pwd)
PATH:=$(PATH):$(TOP)/toolchains/inst/m68k-elf/bin/

.PHONY: all clean toolchain publishdocs

all: toolchain publishdocs
	$(MAKE) -C tools
	$(MAKE) -C musasim
	PATH=$(PATH) $(MAKE) -C libunagipai clean
	PATH=$(PATH) $(MAKE) -C libunagipai install
	$(MAKE) -C examples
clean:
	$(MAKE) -C tools clean	
	$(MAKE) -C libunagipai clean	
	$(MAKE) -C musasim clean
	$(MAKE) -C examples clean

toolchain: 
	cd toolchains && ./mkchain.sh m68k-elf


publishdocs:
	- rm -r docs/*
	$(MAKE) -C musasim docs
	cp -a musasim/docs docs/musasim
	PATH=$(PATH) $(MAKE) -C libunagipai docs
	cp -a libunagipai/docs docs/libunagipai
	
