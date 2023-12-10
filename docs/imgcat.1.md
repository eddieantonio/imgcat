% IMGCAT(1) imgcat User Manual | meow

# NAME

**imgcat** - cat, but for images

# SYNOPSIS

| **imgcat**  **\[options]** _image_
| **imgcat**  **\[options]** < _image_

# DESCRIPTION

**imgcat** prints an image to your terminal screen.

The image can either be provided as a command line argument, or can be
piped in through `stdin`.

By default, the image is printed at the full width and color depth
detected for your terminal. This can be overridden using **-w** to
adjust the maximum width or **-R** to prevent resizing, even if the
image is too big to fit in the terminal; and **-d** to explicitly set
the color depth. You may also use **-r** to adjust the height ("r" for
"number of rows"). The image will be scaled without affecting the aspect
ratio, unless **-P** is provided.

Setting **-H** enables the use of half-height block drawing characters
(as opposed to "full height" spaces used in the default mode). This
effectively doubles the vertical resolution of the terminal, and thus
the image on your terminal will appear less "squished", and have a greater
pixel resolution. Note that this effect works best on terminals that
output using a bitmapped font, and have a line height of exactly 100%
([example][bitmapped-H]). Using anti-aliased fonts may
distort the blocks, and using a taller line height will make the
half-height block cover *less* than half of the block, further
distorting the image ([example][bad-H]). Your millage may vary.

If the output is not a terminal (that is, output is redirected to
a file, or piped into another program), then the image is **not**
resized and the color depth is set to 8 colors. Overriding both width,
height, and color depth still work.

__Make sure your `TERM` environment variable is set to a terminfo with
the full capabilities of you terminal!__ See **Troubleshooting** if
you're having a problem with this.

[bitmapped-H]: https://git.io/vhW5F
[bad-H]: https://git.io/vhW5d

## Options

**-d** _MODE_, **--depth**=_MODE_
  ~ Explicitly set the output color depth to one of **ansi**, **8**
  (alias of **ansi**), **256**, **24bit**, **true** (alias of **24bit**)
  or **iterm**. If not provided, the output color depth will be inferred
  with `tput colors`.

**-h**, **--help**
  ~ Show common options and quit.

**-H**, **--half-height**
 ~ Prints half-height blocks to double the vertical resolution, and make
 for a less squished image. Works best when each character cell is
 exactly twice as tall as it is wide and when your terminal is using
 a bitmapped font.

**-r** _ROWS_, **--height**=_ROWS_
  ~ Resizes the image to _ROWS_ characters high.
  Does nothing if **--no-resize** is provided. Maintains the original image's
  aspect ratio if **--width** is NOT provided.

**-P**, **--no-preserve-aspect-ratio**
  ~ Does not preserve aspect ratio during image resizing.

**-R**, **--no-resize**
  ~ Does not resize the image to fit the terminal's width. Overrides
  both **--width** and **--height**.

**-v**, **--version**
  ~ Show version and quit.

**-w** _COLS_, **--width**=_COLS_
  ~ Shrink the image to _COLS_ characters wide.
  Does nothing if **--no-resize** is provided, or if the image is
  already as small as the provided width. Maintains the original image's
  aspect ratio if **--height** is NOT provided.

**--8**, **--ansi**
  ~ Set the output colour depth to 8. Same as **--depth=8**.

**--256**
  ~ Set the output colour depth to 256. Same as **--depth=256**.

**--24bit**, **--true**
  ~ Set the output colour depth to 24 bit or “true” color.
  Same as **--depth=24bit**.

**--iterm2**
  ~ Set the output to iTerm inline image mode. Same as
  **--depth=iterm2**.


## iTerm2 3.0

**imgcat** supports iTerm2's full-color inline images (use **-d iterm**
to explicitly enable this). However, iTerm2 is bundled with a script
that is also called **imgcat**! To use this program in preference to the
script bundled with iTerm2, edit your shell's configuration file such
that, after the line that sources iTerm2's shell integration and
utilities, you unalias its built-in script:

    unalias imgcat

## Troubleshooting

First, test how many colors your terminal can output:

    $ tput colors
    256

If this is not what you expect, you might want to change your `TERM`
environment variable. For example, iTerm2 users will probably want the
following:

    $ export TERM=xterm-256color

However, it's better that your terminal emulator reports itself with the
proper value for `TERM`. For iTerm2, again, look under the current
profile settings > Terminal > Terminal Emulation > and change "Report
Terminal Type".

# BUGS

See GitHub Issues: <https://github.com/eddieantonio/imgcat/issues>

# AUTHOR

**imgcat** was written by Eddie Antonio Santos <https://eddieantonio.ca/>.
