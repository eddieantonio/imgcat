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

ANY_IMAGE=1px_8_table.png

#           expected output     command
assert_eq   1px_256_table.out   imgcat -d 256 1px_256_table.png
assert_eq   1px_8_table.out     imgcat -d 8 1px_8_table.png

#           command that should fail
assert_fail imgcat --width=-3 "$ANY_IMAGE"
assert_fail imgcat -w mank3y "$ANY_IMAGE"

#           command that should succeed
assert_ok   imgcat --width=12 Xterm_256color_chart.png
