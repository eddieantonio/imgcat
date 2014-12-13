/**
 * print_image and Format.
 */
#ifndef PRINT_IMAGE_H
#define PRINT_IMAGE_H

/* The output color depth/format. */
typedef enum {
    F_8_COLOR, F_256_COLOR, F_RGB_COLOR, F_UNSET
} Format;

/* Prints the image. */
void print_image(const char *filename, int max_width, Format format);

#endif /* PRINT_IMAGE_H */
