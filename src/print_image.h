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
    F_8_COLOR, F_256_COLOR, F_RGB_COLOR, F_UNSET
} Format;

/* Prints the image. Returns true when successful. */
bool print_image(const char *filename, int max_width, Format format);

#endif /* PRINT_IMAGE_H */
