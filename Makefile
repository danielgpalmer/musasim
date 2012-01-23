.PHONY: all clean toolchain

all: toolchain
	$(MAKE) -C tools
	$(MAKE) -C musasim
	$(MAKE) -C musasim docs
	$(MAKE) -C libunagipai install
	$(MAKE) -C m68ktestapp

clean:
	$(MAKE) -C musasim clean
	$(MAKE) -C m68ktestapp clean
	

toolchain: 
	cd toolchains && ./mkchain m68k-elf
