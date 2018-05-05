/*
 * Copyright (c) 2017 Eddie Antonio Santos <easantos@ualberta.ca>
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

#ifndef LOAD_IMAGE_H
#define LOAD_IMAGE_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif

/**
 * Bytes per pixel in the image buffer.
 */
extern const int colour_depth;

/**
 * Contains an image's pixel data and its dimensions. The buffer is always 32
 * bits per pixel: the first three bytes are "colour" channels (either red,
 * blue, green or L*, a*, b*) and the final byte is the alpha channel (not
 * implemented).
 *
 * Usage:
 *
 * Allocate an Image struct, then initialize it using load_image() (by pointer).
 *
 * When finished with the image, call unload_image() (also by pointer).
 */
struct Image {
    int width, height, depth;
    uint8_t *buffer;
};

/**
 * Additional options to pass when loading images.
 */
struct LoadOpts {
    int max_width;
    int desired_height;
};

/**
 * Loads the image at the given filename, into the already allocated Image
 * struct.
 */
bool load_image(const char *filename, struct Image *image, struct LoadOpts*);

/**
 * Frees memory of the loaded image.
 */
void unload_image(struct Image *image);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* LOAD_IMAGE_H */
