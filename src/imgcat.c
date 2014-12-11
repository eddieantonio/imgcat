#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/ioctl.h>

#include "stb_image.h"
#include "stb_image_resize.h"

#include "rgbtree.h"


typedef enum {
    F_8_COLOR, F_256_COLOR, F_RGB_COLOR, F_UNSET
} Format;

/* Global, bite me. */
static struct {
    Format format;      /* Default: 256 colors. */
    bool should_resize; /* Default: yes! */
} options;

static struct {
    int width;
    int height;
    int colors;
    bool isatty;
} terminal_info;



void parse_args(int argc, char **argv);
void print_image(const char *filename, int max_width, Format format);
void determine_terminal_capabilities();

int main(int argc, char **argv) {
    parse_args(argc, argv);
    determine_terminal_capabilities();

    print_image(argv[1], terminal_info.width, F_256_COLOR);

    return 0;
}



void print_image_8(unsigned char *buffer);
void print_image_256(unsigned char *buffer, int width, int height);
void print_image_rgb(unsigned char *buffer);
void do_resize(unsigned char **buffer, int new_width, int* width, int* height);

void print_image(const char *filename, int max_width, Format format) {
    int width, height, depth;
    /* Load with 3 components. */
    unsigned char *buffer = stbi_load(filename, &width, &height, &depth, 3);

    assert(depth == 3 && "Not the right colour depth!");

    /* Maybe do a resize. */
    if (width > max_width) {
        /* Warning: allocates a new buffer and frees the current one. */
        do_resize(&buffer, max_width, &width, &height);
    }
    

    /* That resized buffer? Yeah. Print it. */
    switch (format) {
        case F_8_COLOR:
            assert(0 && "Not implemented!");
            break;
        case F_256_COLOR:
            print_image_256(buffer, width, height);
            break;
        case F_RGB_COLOR:
            assert(0 && "Not implemented!");
            break;
        default:
            assert(0 && "Not a valid format.");
    }

    stbi_image_free(buffer);
}

void determine_terminal_capabilities() {
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

void parse_args(int argc, char **argv) {
    /* TODO: */
    options.format = F_256_COLOR;
    options.should_resize = true;
}

void print_image_256(unsigned char *buffer, int width, int height) {
    int x, y;
    for (y = 0; y < height; y++) {
        /* Print each pixel. */
        for (x = 0; x < width; x++) {
            /* Get the position of the pixel in the image... */
            uint8_t *pixel = buffer + 3 * (x + width * y);
            const RGB_Node *match = rgb_closest_colour(pixel[0], pixel[1], pixel[2]);
            int closest_code = match->id;
            printf("\033[48;5;%03dm ", closest_code);
        }
        /* Finish the line. */
        printf("\033[49m\n");
    }
}

void do_resize(unsigned char **buffer, int new_width, int* width, int* height) {
    unsigned char *original = *buffer, *resized;
    double ratio = ((double) *height) / *width;
    int new_height = ratio * new_width;
    size_t buffer_size = 3 * (new_width * new_height);

    resized = (unsigned char*) malloc(buffer_size);
    stbir_resize_uint8(original, *width, *height, 0,
                       resized, new_width, new_height, 0, 3);

    /* Replace all the originals with our new, fun values. */
    *buffer = resized;
    *width = new_width;
    *height = new_height;

    stbi_image_free(original);
}
