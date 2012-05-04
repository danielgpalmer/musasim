TOP=$(shell pwd)
PATH:=$(PATH):$(TOP)/toolchains/inst/m68k-elf/bin/

.PHONY: all clean toolchain

all: toolchain
	$(MAKE) -C tools
	$(MAKE) -C musasim
	$(MAKE) -C musasim docs
	PATH=$(PATH) $(MAKE) -C libunagipai clean
	PATH=$(PATH) $(MAKE) -C libunagipai install
	PATH=$(PATH) $(MAKE) -C libunagipai docs
	$(MAKE) -C examples
clean:
	$(MAKE) -C tools clean	
	$(MAKE) -C libunagipai clean	
	$(MAKE) -C musasim clean
	$(MAKE) -C m68ktestapp clean

toolchain: 
	cd toolchains && ./mkchain.sh m68k-elf
