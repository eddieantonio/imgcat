% IMGCAT(1) imgcat User Manual | meow

# NAME

**imgcat** - cat, but for images

# SYNOPSIS

| **imgcat** \[**-w** _COLS_|**-R**] \[**-d** _MODE_] \[_options_] _image.ext_

# DESCRIPTION

**imgcat** prints images to your terminal screen.

By default, the image is printed at the full width and color depth
detected for your terminal. These can be overridden using **-w** to
adjust the maximum width or **-R** to prevent resizing, even if the
image is too big to fit in the terminal; and **-d** to explicitly the
set color depth.

__Make sure your `TERM` environment variable is set to a terminfo with
the full capabilities of you terminal!__ See **Troubleshooting** if
you're having a problem with this.

If the output is not a terminal (that is, output is redirected to
a file, or piped into another program), then the image is **not**
resized and the color depth is set to 8 colors. Overriding both width
and color depth still work.

## Options

**-d** _MODE_, **--depth**=_MODE_
  ~ Explicitly set the output color depth to one of **ansi**, **8**
  (alias of **ansi**), **256**, or **iterm**. If not provided, the
  output color depth will be inferred with `tput colors`.

**-R**, **--no-resize**
  ~ Does not resize the image to fit the terminal's width. Overrides
  **--width**.

**-w** _COLS_, **--width**=_COLS_
  ~ Shrink the image to _COLS_ characters wide (maintaining aspect ratio).
  Does nothing if **--no-resize** is provided, or if the image is
  already as small as the provided width.

**--8**, **--ansi**
  ~ Set the output colour depth to 8. Same as **--depth=8**.

**--256**
  ~ Set the output colour depth to 256. Same as **--depth=256**.

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

**imgcat** was written by Eddie Antonio Santos <http://eddieantonio.ca>.
