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

#include <cassert>
#include <cstdlib>
#include <cstdbool>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "cimg_config.h"
#define cimg_verbosity  0
#define cimg_display    0
#include "CImg.h"

#include "load_image.h"
/**
 * red/L*, blue/a*, green/b*, and alpha.
 */
const int COLOUR_DEPTH = 4;

namespace {
void maybe_resize(cimg_library::CImg<unsigned char>&, const LoadOpts&);
}


// TODO: Take "max width", "pixel ratio", "colour space".
// TODO: colour space transformation?
bool load_image(const char *filename, Image *image, struct LoadOpts* options) {
    /* Zero-out the struct. */
    bzero(image, sizeof(struct Image));

    cimg_library::CImg<unsigned char> img;
    try {
        img.assign(filename);
    } catch (cimg_library::CImgIOException& ex) {
        // Could not load the image for some reason.
        return false;
    }

    assert(img.data() != nullptr);

    /* XXX: Set the desired width when the image is too wide  */
    if ((options->desired_width <= 0) &&
            (img.width() > options->max_width)) {
        options->desired_width = options->max_width;
    }

    /* The image may be resized smaller. */
    maybe_resize(img, *options);

    /* Determine the number of bytes of the image. */
    int size = img.width() * img.height() * COLOUR_DEPTH;
    if (size < COLOUR_DEPTH) {
        /* The image is too small! */
        return false;
    }

    static_assert(1 == sizeof (uint8_t),
                  "somehow an octet is not the size of one byte");

    uint8_t* buffer = (uint8_t*) malloc(size);
    if (buffer == nullptr) {
        /* Memory allocation error. */
        return false;
    }

    /* Creates a 32bpp flat buffer copy of the image.
     * The data layout is optimized for linear access to entire pixels,
     * from top-to-bottom, left-to-right per each row.
     * The channels are **interleaved** such that the memory for each
     * individual pixel is cache-local (processor caches don't like it
     * when you hop around memory for each datum).
     */
    uint8_t *pos = buffer;
    const auto greyscale = img.spectrum() < 3;
    for (int y = 0; y < img.height(); y++) {
        for (int x = 0; x < img.width(); x++) {
            if (greyscale) {
                /* Copy the grey channel three times. */
                *pos++ = img(x, y);
                *pos++ = img(x, y);
                *pos++ = img(x, y);
            } else {
                /* Copy each channel independently. */
                *pos++ = img(x, y, 0, 0);
                *pos++ = img(x, y, 0, 1);
                *pos++ = img(x, y, 0, 2);
            }
            /* The alpha channel is currently ignored,
             * so let every pixel be opaque. */
            /* TODO: handle transparency? */
            *pos++ = 0xFF;
        }
    }
    /* Assert we've processed the entire image. */
    assert(pos == buffer + size);

    image->width = img.width();
    image->height = img.height();
    image->buffer = buffer;
    image->depth = COLOUR_DEPTH;
    return true;
}

void unload_image(Image *image) {
    assert(image->buffer != nullptr);
    free(image->buffer);
    image->buffer = nullptr;
    image->width = 0;
    image->height = 0;
}

namespace {
void maybe_resize(cimg_library::CImg<unsigned char>& img, const LoadOpts& options) {
    bool resize_width = options.desired_width > 0;
    bool resize_height = options.desired_height > 0;

    if (resize_width && resize_height) {
        /* Make sure the image is never smaller than 1x1 pixels. */
        int new_width = std::max(options.desired_width, 1);
        int new_height = std::max(options.desired_height, 1);

        /* Resize preserving aspect ratio. */
        if (options.preserve_aspect_ratio) {
            int max_width = new_width;
            int max_height = new_height;
            new_width = img.width();
            new_height = img.height();

            if (new_width > max_width) {
                new_width = max_width;
                double ratio = ((double) img.height()) / img.width();
                /* Scale height, ensuring it's at least 1px. */
                new_height = std::max((int) (ratio * (double) new_width), 1);
            }

            if (new_height > max_height) {
                new_height = max_height;
                double ratio = ((double) img.width()) / img.height();
                /* Scale width, ensuring it's at least 1px. */
                new_width = std::max((int) (ratio * (double) new_height), 1);
            }
        }

        img.resize(new_width, new_height);
    } else if (resize_width) {
        /* Only resize if the image is strictly greater than the source width. */
        if (img.width() <= options.max_width) {
            return;
        }
        int new_width = options.desired_width;
        double ratio = ((double) img.height()) / img.width();
        /* Scale height, ensuring it's at least 1px. */
        int new_height = std::max((int) (ratio * (double) new_width), 1);
        img.resize(new_width, new_height);
    } else if (resize_height) {
        /* Resize without affecting aspect ratio. */
        int new_height = options.desired_height;
        double ratio = ((double) img.width()) / img.height();
        /* Scale width, ensuring it's at least 1px. */
        int new_width = std::max((int) (ratio * (double) new_height), 1);
        img.resize(new_width, new_height);
    }
}
}
