# Delegate all to scons.
all:
	scons
production:
	scons production=true
test: all
	./imgcat 
clean:
	scons -c
.PHONY: all clean production test
