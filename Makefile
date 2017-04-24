# Copyright (c) 2014, 2017, Eddie Antonio Santos <easantos@ualberta.ca>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

BIN = src/imgcat
MAN = docs/imgcat.1

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
	install -d $(BINDIR) $(MANDIR)
	install -s $(BIN) $(BINDIR)
	install -m 644 $(MAN) $(MANDIR)

test: $(BIN)
	tests/run $<

clean:
	$(MAKE) -C src/ clean

.PHONY: all $(BIN) clean install test
