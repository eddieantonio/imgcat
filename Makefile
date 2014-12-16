SRC = src/
BIN = build/imgcat

# Delegate all of these to scons.
all: $(BIN)
$(BIN): $(SRC)
	scons
install: $(SRC)
	scons install production=true
production: $(SRC)
	scons production=true
test: $(BIN)
	$(BIN) -R -d 256 test/1px_256_table.png | diff test/1px_256_table.out -
	$(BIN) -R -d   8 test/1px_8_table.png   | diff test/1px_8_table.out   -
clean:
	scons -c
	$(RM) -r build/
.PHONY: all clean install production test
