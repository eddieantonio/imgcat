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
	$(BIN) -R test/1px_256_table.png | diff test/1px_256_color.txt -
clean:
	scons -c
	$(RM) -r build/
.PHONY: all clean install production test
