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
 * TODO: better documentation:
 * Prints images and stuff. Yeah.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "print_image.h"
#include "load_image.h"

#include "rgbtree.h"

typedef const unsigned char Pixel;
typedef void (*PixelFunc)(Pixel *pixel);

static void image_iterator(struct Image *image, PixelFunc printer);
static void printer_8_color(Pixel *pixel);
static void printer_256_color(Pixel *pixel);

/* The 8 color table. It has 8 colors. */
static const RGB_Tuple ansi_color_table[] = {
    {{  0,   0,   0}}, {{ 128,   0,   0}},
    {{  0, 128,   0}}, {{ 128, 128,   0}},
    {{  0,   0, 128}}, {{ 128,   0, 128}},
    {{  0, 128, 128}}, {{ 128, 128, 128}},
};



bool print_image(const char *filename, int max_width, Format format) {
    struct Image image;
    struct LoadOpts options = {
        .max_width = max_width
    };
    assert(format != F_UNSET);

    /* Load the image. */
    bool success = load_image(filename, &image, &options);

    /* Could not load image. */
    if (!success) {
        return false;
    }

    // TODO: WHAT?
    /* Maybe do a resize. */
    if ((max_width != WIDTH_UNSET) && (image.width > max_width)) {
        fprintf(stderr, "scaling not implemented");
        exit(-1);
    }

    PixelFunc printer = NULL;

    /* That resized buffer? Yeah. Print it. */
    switch (format) {
        case F_8_COLOR:
            printer = printer_8_color;
            break;
        case F_256_COLOR:
            printer = printer_256_color;
            break;
        default:
            assert(0 && "Not a valid format.");
    }

    /* Print the image with the given pixel func. */
    image_iterator(&image, printer);

    unload_image(&image);
    return true;
}


/**
 * Gets a colour match from the global RGB tree.
 */
static void printer_256_color(Pixel *pixel) {
    const RGB_Node *match = rgb_closest_colour(pixel[0], pixel[1], pixel[2]);
    int closest_code = match->id;
    printf("\033[48;5;%03dm ", closest_code);
}

/* Gets a color from the list. Will take same number of steps as the tree
 * version. */
static void printer_8_color(Pixel *pixel) {
    RGB_Tuple target = {{pixel[0], pixel[1], pixel[2]}};
    int i, best_index = 0;
    unsigned int distance, closest;
    closest = rgb_colour_distance(&ansi_color_table[0], &target);

    for (i = 1; i < 8; i++) {
        distance = rgb_colour_distance(&ansi_color_table[i], &target);
        if (distance < closest) {
            closest = distance;
            best_index = i;
        }
    }

    /* It turns out that the 8 color array has the SAME indices as its
     * corresponding ANSI escape sequence. */
    printf("\033[4%1dm ", best_index);
}


static void image_iterator(struct Image *image, PixelFunc printer) {
    int x, y;
    const int width = image->width, height = image->height;
    int color_depth = image->depth;
    unsigned char *buffer = image->buffer;

    for (y = 0; y < height; y++) {
        /* Print each pixel. */
        for (x = 0; x < width; x++) {
            /* Get the position of the pixel in the image... */
            uint8_t *pixel = buffer + color_depth * (x + width * y);
            printer(pixel);
        }
        /* Finish the line. */
        printf("\033[49m\n");
    }
}
