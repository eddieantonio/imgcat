# Copyright (c) 2014–2019, Eddie Antonio Santos <easantos@ualberta.ca>
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

# Outputs
BIN = imgcat
MAN = docs/imgcat.1

# Install stuff
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

include config.mk
include cstandard.mk

# the -M* options produce .d files in addition to .o files,
# to keep track of header dependencies (see: $(DEPS)).
OUTPUT_OPTION = -MMD -MP -o $@

# Use the C++ compiler to link, because we're using one C++ file!
LD = $(CXX)

# CImg requires pthread, for some reason
LDLIBS = $(LIBS) -ltermcap -lm -lpthread

# Get the source files.
SOURCES = $(wildcard src/*.c) $(wildcard src/*.cc)
OBJS = $(addsuffix .o,$(basename $(SOURCES)))
DEPS = $(OBJS:.o=.d)
INCLUDES = ./CImg
INCLUDES_PARAMS =$(foreach d, $(INCLUDES), -I$d)


all: $(BIN) $(MAN)

install: $(BIN) $(MAN)
	install -d $(BINDIR) $(MANDIR)
	install -s $(BIN) $(BINDIR)
	install -m 644 $(MAN) $(MANDIR)

test: $(BIN)
	tests/run $<

clean:
	$(RM) $(BIN) $(OBJS)

-include $(DEPS)

$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.1: %.1.md
	$(PANDOC) --standalone --from=markdown-smart --to=man $(PANDOCFLAGS) \
		-Vdate='$(shell date +'%B %d, %Y')' $< -o $@


# XXX: The CImg.h file uses arr['char'] as subscripts, which Clang doesn't
# like, so enable this flag JUST for the file that includes it!
src/load_image.o: CXXFLAGS+=-Wno-char-subscripts $(INCLUDES_PARAMS)
src/load_image.o:

# This is a bit uncouth, but Make can do all that autoconf stuff for us.
config.mk: configure config.mk.in
	./$<

CImg/CImg.h:
	git submodule update --init

.PHONY: all clean install test
