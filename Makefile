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
	$(BIN) -d 256 test/1px_256_table.png | diff test/1px_256_table.out -
	$(BIN) -d   8 test/1px_8_table.png   | diff test/1px_8_table.out   -
	$(BIN) /dev/null 2> /dev/null ; test $$? -ne 0
clean:
	scons -c
	$(RM) -r build/
.PHONY: all clean install production test
