# Enable optimizations in production mode.
ifdef production
CFLAGS += -O2
else
CFLAGS += -g
endif

# Use C11 -- workaround for older versions of GCC.
ifeq ($(firstword $(CC)),gcc)
CFLAGS += -std=c1x
else
CFLAGS += -std=c11
endif

LDLIBS = -lm

BIN = imgcat
SOURCES = imgcat stb_image stb_image_resize rgbtree print_image
OBJS := $(addsuffix .o,$(SOURCES))


$(BIN): $(OBJS)

clean:
	$(RM) $(BIN) $(OBJS)

.PHONY: clean
