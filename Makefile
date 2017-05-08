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

# Outputs
BIN = imgcat
MAN = docs/imgcat.1

# Install stuff
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

include config.mk

# TODO: use autoconf to figure this out.
# Use C11 and C++11 --- workarounds for older versions of GCC.
ifeq ($(firstword $(CC)),gcc)
CFLAGS += -std=c1x
else
CFLAGS += -std=c11
endif
ifeq ($(firstword $(CXX)),g++)
CXXFLAGS += -std=c++0x
else
CXXFLAGS += -std=c++11
endif

# TODO: AC_CHECK_HEADERS/AC_CHECK_LIBS
# Add conditional support for libpng.
ifneq ($(shell pkg-config --modversion libpng),)
CIMG_CXXFLAGS += $(shell pkg-config --cflags libpng) -Dcimg_use_png
LDLIBS += $(shell pkg-config --libs libpng)
endif

# Enable optimizations in production mode.
# TODO: do something about this...?
ifdef production
CFLAGS += -O2
CXXFLAGS += -O2
else
CFLAGS += -g
CXXFLAGS += -g
endif

# Add any special flags for CImg
# TODO: Create a cimg_config.h
#load_image.o: CXXFLAGS += $(CIMG_CXXFLAGS) -Dcimg_use_jpeg
#load_image.o:

# Always link with libjpeg; CImg requires pthread, for some reason
# TODO: augment this from autoconf
LDLIBS = -ljpeg -lm -ltermcap -lpthread

# Get the source files.
SOURCES = $(wildcard src/*.c) $(wildcard src/*.cc)
OBJS = $(addsuffix .o,$(basename $(SOURCES)))
DEPS = $(OBJS:.o=.d)


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
	$(PANDOC) --standalone --to=man $(PANDOCFLAGS) \
		-Vdate='$(shell date +'%B %d, %Y')' $< -o $@

.PHONY: all clean install test
