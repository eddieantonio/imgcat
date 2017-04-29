# Copyright (c) 2017, Eddie Antonio Santos <easantos@ualberta.ca>
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

CFLAGS ?= -Wall -Werror
CXXFLAGS ?= -Wall -Werror
LDLIBS = -ljpeg -lm -pthread
CIMG_CXXFLAGS = -Dcimg_use_jpeg

# Enable optimizations in production mode.
ifdef production
CFLAGS += -O2
CXXFLAGS += -O2
else
CFLAGS += -g
CXXFLAGS += -g
endif

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


# Add conditional support for libpng.
ifneq ($(shell pkg-config --modversion libpng),)
CIMG_CXXFLAGS += $(shell pkg-config --cflags libpng) -Dcimg_use_png
LDLIBS += $(shell pkg-config --libs libpng)
endif

BIN = imgcat
SOURCES = imgcat load_image print_image rgbtree
OBJS := $(addsuffix .o,$(SOURCES))


$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@


# Add any special flags for CImg
load_image.o: CXXFLAGS += $(CIMG_CXXFLAGS)
load_image.o: load_image.cc load_image.h CImg.hpp


clean:
	$(RM) $(BIN) $(OBJS)

.PHONY: clean
