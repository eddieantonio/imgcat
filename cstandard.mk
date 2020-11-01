# Determine supported standard
ifeq ($(CC),gcc)
CSTD = -std=c1x
CXXSTD = -std=c++0x
else
CSTD = -std=c11
CXXSTD = -std=c++11
endif

CFLAGS += $(CSTD) -Wall
CXXFLAGS += $(CXXSTD) -Wall
