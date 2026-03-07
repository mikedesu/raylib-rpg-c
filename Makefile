.PHONY: all game clean tests index.html

all game clean tests index.html:
	$(MAKE) -C src $@ 

