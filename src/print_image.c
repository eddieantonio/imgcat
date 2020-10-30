/*
 * Copyright (c) 2014–2018, Eddie Antonio Santos <easantos@ualberta.ca>
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
 * print_image() prints an image to the screen, in whatever format is most
 * suitable.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "print_image.h"
#include "load_image.h"

#include "rgbtree.h"

enum {
    /* PixelFuncs writes "parameter bytes" to a provided buffer that is
     * **at most** this big.
     * Parameter bytes are the bytes that go between the Command Sequence
     * Initiator (CSI, a.k.a, "\033[") and the "m" at the end.
     */
    MAX_ESC_SEQUENCE_LEN = sizeof("38;2;000;000;000;48;2;000;000;000")
};

enum layer {
    BACKGROUND, FOREGROUND
};

typedef const unsigned char Pixel;
/**
 * A pixel function takes in a pixel and places an escape sequence within a
 * buffer. It must return a pointer to characters allocated within the
 * provided buffer.
 */
typedef const char* (*PixelFunc)(Pixel *pixel, char sequence[], enum layer);

static bool iterm2_passthrough(PrintRequest *request);
static bool print_base64(const char *filename);
static bool print_iterate(PrintRequest *request);
static void half_height_image_iterator(struct Image *image, PixelFunc printer);
static void image_iterator(struct Image *image, PixelFunc printer);
static void print_osc();
static void print_st();
static const char* printer_true_color(Pixel *pixel, char sequence[], enum layer);
static const char* printer_256_color(Pixel *pixel, char sequence[], enum layer);
static const char* printer_8_color(Pixel *pixel, char sequence[], enum layer);

/* The 8 color table. It has 8 colors. */
static const RGB_Tuple ansi_color_table[] = {
    {{  0,   0,   0}}, {{ 128,   0,   0}},
    {{  0, 128,   0}}, {{ 128, 128,   0}},
    {{  0,   0, 128}}, {{ 128,   0, 128}},
    {{  0, 128, 128}}, {{ 128, 128, 128}},
};


bool print_image(PrintRequest *request) {
    if (request->format == F_ITERM2) {
        return iterm2_passthrough(request);
    } else {
        /* Delegate to the "pixel iterator" approach. */
        return print_iterate(request);
    }
}


static bool print_iterate(PrintRequest *request) {
    struct Image image;
    const char *filename = request->filename;
    Format format = request->format;
    struct LoadOpts options = {
        .max_width = request->max_width,
        .max_height = request->max_height,
        .desired_width = request->desired_width,
        .desired_height = request->desired_height,
    };
    assert(format != F_UNSET);

    /* Load the image, and potentially rescale it. */
    bool success = load_image(filename, &image, &options);

    /* Could not load image. */
    if (!success) {
        return false;
    }

    PixelFunc printer = NULL;

    /* That resized buffer? Yeah. Print it. */
    switch (format) {
        case F_TRUE_COLOR:
            printer = printer_true_color;
            break;
        case F_256_COLOR:
            printer = printer_256_color;
            break;
        case F_8_COLOR:
            printer = printer_8_color;
            break;
        default:
            assert(0 && "Not a valid format.");
    }

    /* Print the image with the given pixel func. */
    if (request->half_height) {
        half_height_image_iterator(&image, printer);
    } else {
        image_iterator(&image, printer);
    }

    unload_image(&image);
    return true;
}

/**
 * Pass-through to iTerm2's inline image feature.
 *
 * https://iterm2.com/documentation-images.html
 *
 * Based on iTerm's included script, **also** called imgcat:
 * https://raw.githubusercontent.com/gnachman/iTerm2/master/tests/imgcat
 */
static bool iterm2_passthrough(PrintRequest *request) {
    print_osc();
    printf("1337;File=inline=1");

    assert(request->desired_width == WIDTH_UNSET || request->desired_width > 0);
    assert(request->desired_height == HEIGHT_UNSET || request->desired_height > 0);
    if (request->desired_width != WIDTH_UNSET) {
        printf(";width=%d", request->desired_width);
    }
    if (request->desired_height != HEIGHT_UNSET) {
        printf(";height=%d", request->desired_height);
    }

    printf(":");
    print_base64(request->filename);

    print_st();
    return true;
}

/**
 * Iterates through the image, x, then y,
 */
static void image_iterator(struct Image *image, PixelFunc printer) {
    char sequence[MAX_ESC_SEQUENCE_LEN];
    const int width = image->width, height = image->height;
    const int color_depth = image->depth;
    unsigned char *pixels = image->buffer;

    for (int y = 0; y < height; y++) {
        /* Print each pixel. */
        for (int x = 0; x < width; x++) {
            /* Get the position of the first channel of the pixel. */
            uint8_t *pixel = pixels + color_depth * (x + width * y);
            /* Delegate to the provided printer. */
            const char* parameter_bytes = printer(pixel, sequence, BACKGROUND);

            assert(parameter_bytes >= sequence);
            assert(parameter_bytes < sequence + MAX_ESC_SEQUENCE_LEN);
            printf("\033[%sm ", parameter_bytes);
        }
        /* Finish the line. */
        /* TODO: this can go at the very end. */
        printf("\033[49m\n");
    }
}

/**
 * Iterates through the image, two rows at a time, two pixels per cell.
 */
static void half_height_image_iterator(struct Image *image, PixelFunc printer) {
    char upper_half[MAX_ESC_SEQUENCE_LEN], lower_half[MAX_ESC_SEQUENCE_LEN];
    const int width = image->width, height = image->height;
    const int color_depth = image->depth;
    unsigned char *pixels = image->buffer;

    /* Increment two lines at a time. Focus on the BOTTOM of the two lines
     * (because if the bottom line is valid, then we know there must be a line
     * above it. */
    for (int y = 1; y < height; y += 2) {
        /* Print each pixel. */
        for (int x = 0; x < width; x++) {
            /* Get the position of the first channel of the pixel. */
            uint8_t *top_pixel = pixels + color_depth * (x + width * (y - 1));
            uint8_t *bottom_pixel = pixels + color_depth * (x + width * y);

            printf("\033[%s;%sm▀",
                    printer(top_pixel, upper_half, FOREGROUND),
                    printer(bottom_pixel, lower_half, BACKGROUND));
        }
        /* Finish the line by reseting the background and foreground colors.
         * If you don't reset the background color, the color "spills" to the
         * end of the line. */
        printf("\033[39;49m\n");
    }
}

/**
 * Convert the pixel values to an escape sequence directly
 */
static const char* printer_true_color(Pixel *pixel, char sequence[], enum layer layer) {
    char category = layer == FOREGROUND ? '3' : '4';
    snprintf(sequence, MAX_ESC_SEQUENCE_LEN,
            "%c8;2;%03d;%03d;%03d", category, pixel[0], pixel[1], pixel[2]);
    return sequence;
}

/**
 * Gets a colour match from the global RGB tree.
 */
static const char* printer_256_color(Pixel *pixel, char sequence[], enum layer layer) {
    const RGB_Node *match = rgb_closest_colour(pixel[0], pixel[1], pixel[2]);
    int closest_code = match->id;
    char category = layer == FOREGROUND ? '3' : '4';
    snprintf(sequence, MAX_ESC_SEQUENCE_LEN,
            "%c8;5;%03d", category, closest_code);
    return sequence;
}

/* Gets a color from the list. Will take same number of steps as the tree
 * version. */
static const char* printer_8_color(Pixel *pixel, char sequence[], enum layer layer) {
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
    int ansi_code = (layer == FOREGROUND ? 30 : 40) + best_index;
    snprintf(sequence, MAX_ESC_SEQUENCE_LEN, "%2d", ansi_code);
    return sequence;
}

static void print_base64_char(uint8_t c) {
    static const char b64_encode_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    uint8_t index = c & 0b00111111;
    putchar(b64_encode_table[index]);
}

/**
 * Open the file and prints its contents in "canonical" base64.
 * See Table 1 in RFC4648: https://tools.ietf.org/html/rfc4648#page-6
 */
static bool print_base64(const char *filename) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        return false;
    }
    int c;
    uint8_t leftover_bits = 0;

    enum { byte_0, byte_1, byte_2, max_states } state = byte_0;

    /* Convert each character into base64. */
    while ((c = fgetc(file)) != EOF) {
        uint8_t lower, upper;
        switch (state) {
            case byte_0:
                print_base64_char(c >> 2);
                break;
            case byte_1:
                lower = (leftover_bits & 0b11) << 4;
                upper = (c & 0b11110000) >> 4;
                print_base64_char(upper | lower);
                break;
            case byte_2:
                upper = (leftover_bits & 0b00001111) << 2;
                lower = (c & 0b11000000) >> 6;
                print_base64_char(upper | lower);
                print_base64_char(c & 0b00111111);
                break;
            default:
                assert(false);
        }
        /* Save the leftover bits. */
        leftover_bits = c;
        /* Move to the next state. */
        state = (state + 1) % max_states;
    }

    /* Pad the end, if needed. */
    uint8_t upper = 0;
    switch (state) {
        case byte_0:
            /* No padding necessary: last character ended on byte boundary. */
            break;
        case byte_1:
            upper = (c & 0b11110000) >> 4;
            print_base64_char(upper);
            printf("==");  /* padding */
            break;
        case byte_2:
            upper = (leftover_bits & 0b00001111) << 2;
            print_base64_char(upper);
            printf("=");  /* padding */
            break;
        default:
            assert(false);
    }

    fclose(file);
    return true;
}

/**
 * Print the start of an operating system command (OSC).
 */
static void print_osc() {
    printf("\033]");
}

/**
 * Print the string terminator (ST), which in iTerm's case is simply the ASCII
 * bell.
 */
static void print_st() {
    printf("\007\n");
}
