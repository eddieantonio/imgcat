/**
 * Prints images and stuff. Yeah.
 *
 */

#include <assert.h>
#include <stdlib.h>

#include "print_image.h"

#include "rgbtree.h"
#include "stb_image.h"
#include "stb_image_resize.h"

/* Bundles all of the image stuff in one nice struct. */
typedef struct {
    unsigned char *buffer;
    int width;
    int height;
    int depth;
} Image;

typedef const unsigned char Pixel;
typedef void (*PixelFunc)(Pixel *pixel);

static void image_iterator(Image *image, PixelFunc printer);
static void printer_8_color(Pixel *pixel);
static void printer_256_color(Pixel *pixel);
static void print_image_rgb(Image *image);
static void reallocate_and_resize(Image *image, int new_width);

/* The 8 color table. It has 8 colors. */
static const RGB_Tuple ansi_color_table[] = {
    {{  0,   0,   0}}, {{ 128,   0,   0}},
    {{  0, 128,   0}}, {{ 128, 128,   0}},
    {{  0,   0, 128}}, {{ 128,   0, 128}},
    {{  0, 128, 128}}, {{ 128, 128, 128}},
};



void print_image(const char *filename, int max_width, Format format) {
    Image image;

    /* Load with any number of components. */
    image.buffer = stbi_load(filename, &image.width,
                                       &image.height,
                                       &image.depth, 0);

    /* TODO: Error if cannot load. */
    assert(image.buffer != NULL && "Could not load image!");

    /* Maybe do a resize. */
    if ((max_width != WIDTH_UNSET) && (image.width > max_width)) {
        /* Warning: allocates a new buffer and frees the current one. */
        reallocate_and_resize(&image, max_width);
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
        case F_RGB_COLOR:
            assert(0 && "Not implemented!");
            break;
        default:
            assert(0 && "Not a valid format.");
    }

    /* Print the image with the given pixel func. */
    image_iterator(&image, printer);

    free(image.buffer);
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
    RGB_Tuple target = { pixel[0], pixel[1], pixel[2] };
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


static void image_iterator(Image *image, PixelFunc printer) {
    int x, y;
    int width = image->width, height = image->height;
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


static void reallocate_and_resize(Image *image, int new_width) {
    unsigned char *original = image->buffer;
    unsigned char *resized;
    double ratio = ((double) image->height) / image->width;
    int new_height = ratio * new_width;
    int color_depth = image->depth;

    size_t buffer_size = color_depth * (new_width * new_height);

    resized = (unsigned char*) malloc(buffer_size);
    stbir_resize_uint8(original, image->width, image->height, 0,
                       resized, new_width, new_height, 0, color_depth);

    /* Replace all the originals with our new, fun values. */
    image->buffer = resized;
    image->width = new_width;
    image->height = new_height;

    stbi_image_free(original);
}
