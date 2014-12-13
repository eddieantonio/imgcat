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

static void print_image_8(Image *image);
static void print_image_256(Image *image);
static void print_image_rgb(Image *image);
static void reallocate_and_resize(Image *image, int new_width);

void print_image(const char *filename, int max_width, Format format) {
    Image image;

    /* Load with any number of components. */
    image.buffer = stbi_load(filename, &image.width,
                                       &image.height,
                                       &image.depth, 0);

    /* TODO: Error if cannot load. */
    assert(image.buffer != NULL && "Could not load image!");

    /* Maybe do a resize. */
    if (image.width > max_width) {
        /* Warning: allocates a new buffer and frees the current one. */
        reallocate_and_resize(&image, max_width);
    }

    /* That resized buffer? Yeah. Print it. */
    switch (format) {
        case F_8_COLOR:
            assert(0 && "Not implemented!");
            break;
        case F_256_COLOR:
            print_image_256(&image);
            break;
        case F_RGB_COLOR:
            assert(0 && "Not implemented!");
            break;
        default:
            assert(0 && "Not a valid format.");
    }

    free(image.buffer);
}

void print_image_256(Image *image) {
    int x, y;
    int width = image->width, height = image->height;
    int color_depth = image->depth;
    unsigned char *buffer = image->buffer;

    for (y = 0; y < height; y++) {
        /* Print each pixel. */
        for (x = 0; x < width; x++) {
            /* Get the position of the pixel in the image... */
            uint8_t *pixel = buffer + color_depth * (x + width * y);
            const RGB_Node *match = rgb_closest_colour(pixel[0], pixel[1], pixel[2]);
            int closest_code = match->id;
            printf("\033[48;5;%03dm ", closest_code);
        }
        /* Finish the line. */
        printf("\033[49m\n");
    }
}

void reallocate_and_resize(Image *image, int new_width) {
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

