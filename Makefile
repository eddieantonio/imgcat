all:
	scons
production:
	scons production=true
clean:
	scons -c
.PHONY: all clean production
