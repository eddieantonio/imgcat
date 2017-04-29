/*
 * Copyright (c) 2014, 2017, Eddie Antonio Santos <easantos@ualberta.ca>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * print_image and Format.
 */
#ifndef PRINT_IMAGE_H
#define PRINT_IMAGE_H

#include <stdbool.h>

/* The width has been left unspecified. */
#define WIDTH_UNSET     -1

/* The output color depth/format. */
typedef enum {
    F_8_COLOR, F_256_COLOR, F_ITERM2, F_UNSET
} Format;

/* Prints the image. Returns true when successful. */
bool print_image(const char *filename, int max_width, Format format);

#endif /* PRINT_IMAGE_H */
