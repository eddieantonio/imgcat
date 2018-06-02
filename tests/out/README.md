Expected output of imgcat(1)
============================

This directory contains the expected output of inputting an image and
imgcat(1)'s output. The top-level directories are the input image name
(stored in ../img), and the directory contains a number of expected
stdout produced by imgcat(1). The name of the expected output is the
color format that should be generated, optionally followed by the
terminal width and height. An "N" for either width or height means that
is is unspecified, or can be safely ignored.

Using regular cat(1) with any of the `*.bin` files should output the
image on the terminal!

An "H" after the color format indicates that the output is made for
half-height blocks (like ▀).

    .
    ├── {image_name}
        ├── {color-format}{H?}.bin
        └── {color-format}{H?}.{width}x{height}.bin
