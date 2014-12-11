SRC = src/
BIN = build/imgcat

# Delegate all of these to scons.
all: $(BIN)
$(BIN): $(SRC)
	scons
install: $(SRC)
	scons install
production: $(SRC)
	scons production=true
test: $(BIN)
	$(BIN) test/small_xterm_256color_chart.png
clean:
	scons -c
	$(RM) -r build/
.PHONY: all clean install production test
