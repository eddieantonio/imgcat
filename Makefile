BIN = src/imgcat
MAN = doc/imgcat.1

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

# Delegate all of these to scons.
all: $(BIN)

$(BIN):
	$(MAKE) -C $(dir $@)

$(MAN): 
	$(MAKE) -C $(dir $@)

install: production=true
install: $(BIN) $(MAN)
	install -d $(BIN) $(BINDIR)
	install -d -m 644 $(MAN) $(MANDIR)

production: production=true
production: $(SRC)
	scons production=true

test: $(BIN)
	$(BIN) -d 256 test/1px_256_table.png | diff test/1px_256_table.out -
	$(BIN) -d   8 test/1px_8_table.png   | diff test/1px_8_table.out   -
	$(BIN) /dev/null 2> /dev/null ; test $$? -ne 0
	$(BIN) --width=72 test/any >  /dev/null
	$(BIN) --width=-3 test/any 2> /dev/null ; test $$? -ne 0
	$(BIN) -w hurrrrr test/any 2> /dev/null ; test $$? -ne 0

clean:
	$(MAKE) -C src/ clean
	$(MAKE) -C doc/ clean

.PHONY: all clean install production test
