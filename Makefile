# Delegate all to scons.
all: imgcat
build/imgcat:
	scons
imgcat: build/imgcat
	ln -sf $< $@
production:
	scons production=true
test: imgcat
	./imgcat 
clean:
	scons -c
.PHONY: all build/imgcat clean production test
