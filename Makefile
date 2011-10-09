all:
	$(MAKE) -C musasim
	$(MAKE) -C m68ktestapp

clean:
	$(MAKE) -C musasim clean
	$(MAKE) -C m68ktestapp clean
	
