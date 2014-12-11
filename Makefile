# Delegate all to scons.
all: imgcat

build/imgcat:
	scons
imgcat: build/imgcat
	ln -s $< $@
production:
	scons production=true
test: imgcat
	./imgcat 
clean:
	scons -c
.PHONY: all clean production test
