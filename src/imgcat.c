#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/ioctl.h>

#include <getopt.h>

#include "rgbtree.h"
#include "stb_image.h"
#include "stb_image_resize.h"


/* The output color depth/format. */
typedef enum {
    F_8_COLOR, F_256_COLOR, F_RGB_COLOR, F_UNSET
} Format;

#define WIDTH_UNSET -1

/* Global, bite me. */
static struct {
    Format format;
    bool should_resize;
    int width;
} options = {
    .format = F_256_COLOR,      /* Default: 256 colors. */
    .should_resize = true,      /* Default: yes! */
    .width = WIDTH_UNSET,
};

static struct {
    int width;
    int height;
    int colors;
    bool isatty;
} terminal_info;

typedef struct {
    unsigned char *buffer;
    int width;
    int height;
    int depth;
} Image;

static int parse_args(int argc, char **argv);
static void print_image(const char *filename, int max_width, Format format);
static void determine_terminal_capabilities();


int main(int argc, char **argv) {
    int image_name = parse_args(argc, argv);
    determine_terminal_capabilities();

    print_image(argv[image_name], terminal_info.width, F_256_COLOR);

    return 0;
}


void print_image_8(Image *image);
void print_image_256(Image *image);
void print_image_rgb(Image *image);
void reallocate_and_resize(Image *image, int new_width);

static void print_image(const char *filename, int max_width, Format format) {
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

static void determine_terminal_capabilities() {
    FILE *tput;
    int stdout_fd = fileno(stdout);
    struct winsize ws;
    int colors;

    if (!isatty(stdout_fd)) {
        terminal_info.isatty = false;
        return;
    }

    terminal_info.isatty = true;

    /* Get the current window size. */
    assert(ioctl(stdout_fd, TIOCGWINSZ, &ws) != -1);
    terminal_info.width = ws.ws_col;
    terminal_info.height = ws.ws_row;

    /* I don't really know of a better way to do this other than invoke tput. */
    assert((tput = popen("tput colors", "r")) != NULL);
    assert(fscanf(tput, "%d", &colors) == 1);
    assert(pclose(tput) != -1);
    terminal_info.colors = colors;
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



static void usage(FILE *dest) {
    fprintf(dest, "Usage: \timgcat [-w width|-R] [-d depth] image.jpg\n\n");

}

/* Long options */
static struct option long_options[] = {
     { "no-resize",     no_argument,            NULL,           'R' },
     { "width",         required_argument,      NULL,           'w' },
     { "depth",         required_argument,      NULL,           'd' },
     { NULL,            0,                      NULL,           0 }
};

static Format parse_format(const char *arg);

static int parse_args(int argc, char **argv) {
    int c;

    while (1) {
        c = getopt_long(argc, argv, "w:d:Rh", long_options, NULL);
        if (c == -1)
            break;

        switch (c) {
            case 'w':
                options.should_resize = false;
                break;

            case 'd':
                options.format = parse_format(optarg);
                if (options.format == F_UNSET) {
                    fprintf(stderr, "Unknown output format: %s\n", optarg);
                    /* This funky control flow sends us to print usage. */
                    c = -1;
                } else {
                    break;
                }

            case 'R':
                options.should_resize = false;
                break;

            case 'h':
                usage(stdout);
                exit(0);
                break;

            case '?':
                /* Unknown option. */
                fprintf(stderr, "Unknown option: %s\n", argv[optind]);

            default:
                usage(stderr);
                exit(-1);
        }
    }

    return optind;
}

static Format parse_format(const char *arg) {
    if (strncmp(arg, "256", 4)) {
        return F_256_COLOR;
    } else if (strncmp(arg, "8", 2) || strncmp(arg, "ansi", 5)) {
        return F_8_COLOR;
    } else if (strncmp(arg, "rgb", 4)) {
        return F_UNSET;
    }
    return F_UNSET;
}
