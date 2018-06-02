#!/usr/bin/env python

# Copyright (c) 2014–2018, Eddie Antonio Santos <easantos@ualberta.ca>
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

"""
Generates images for tests.

    pip install --user PyYAML Pillow

 - 1px_256_table.png
       A PNG where every pixel EXACTLY cooresponds to a
       256 color value.

 - 1px_256_table.jpg
        A JPEG where every pixel ATTEMPTS to cooresponds to
        a 256 color value. But they won't.
"""

import os
import sys
import yaml

from PIL import Image

TEST_DIR = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'test')


WIDTH = 12
# 256 color cube, greyscale ramp, 16 colors
HEIGHT = 18 + 2 + 2

with open('xterm-256color.yaml') as f:
    _COLORS = yaml.load(f)


def tupleize(name, x):
    i = lambda x: int(x, 16)
    rgb = (i(x[1:3]), i(x[3:5]), i(x[5:7]))
    return name, rgb

COLORS = [
    [tupleize(*arg) for arg in _COLORS[':xterm256']],
    [tupleize(*arg) for arg in _COLORS[':xtermGreyscale']],
    [tupleize(*arg) for arg in _COLORS[':xterm16']]
]


def main(*args):
    im = Image.new('RGB', (WIDTH, HEIGHT), (0, 0, 0))

    row = 0
    col = 0
    base_row = 0

    # 18 rows of 6**3 color cube.
    for _, val in COLORS[0]:
        im.putpixel((col, row), val)

        row += 1
        # Next column...
        if (row % 6) == 0:
            col += 1
            if (col % 12) == 0:
                base_row += 6
                col = 0
            row = base_row

    assert row == 18

    # 2 rows of greyscale
    for _, val in COLORS[1]:
        im.putpixel((col, row), val)

        col += 1
        # Next row...
        if (col % 12) == 0:
            row += 1
            col = 0

    assert row == 20

    # 2 rows of 16 color.
    for _, val in COLORS[2]:
        im.putpixel((col, row), val)

        col += 1
        # Next row...
        if (col % 8) == 0:
            row += 1
            col = 0

    # Save 'em
    im.save(os.path.join(TEST_DIR, '1px_256_table.png'))
    im.save(os.path.join(TEST_DIR, '1px_256_table.jpg'), quality=40)


if __name__ == '__main__':
    sys.exit(main(*sys.argv))
