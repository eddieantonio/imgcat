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

LDLIBS = -lm -lpthread

BIN = imgcat
SOURCES = imgcat load_image print_image rgbtree
OBJS := $(addsuffix .o,$(SOURCES))


$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	$(RM) $(BIN) $(OBJS)

.PHONY: clean
