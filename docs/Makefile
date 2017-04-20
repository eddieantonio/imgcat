MAN = imgcat.1

# Build only if Pandoc is installed.
ifeq ($(shell command -v pandoc),)
PANDOC = true
else
PANDOC = pandoc 
endif

PANDOCFLAGS =

$(MAN):

%.1: %.1.md
	$(PANDOC) --standalone --to=man $(PANDOCFLAGS) \
		-Vdate='$(shell date +'%B %d, %Y')' $< -o $@
