.PHONY: all clean

all:
	$(MAKE) -C tools
	$(MAKE) -C musasim
	$(MAKE) -C musasim docs
	$(MAKE) -C m68ktestapp
	$(MAKE) -C libunagipai install

clean:
	$(MAKE) -C musasim clean
	$(MAKE) -C m68ktestapp clean
	
