#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <sys/ioctl.h>

#include "stb_image.h"


typedef enum {
    F_8_COLOR, F_256_COLOR, F_RGB_COLOR, F_UNSET
} Format;

/* Global, bite me. */
static struct {
    Format format;      /* Default: 256 colors. */
    bool should_resize; /* Default: yes! */
} options;

void parse_args(int argc, char **argv);
void print_image(const char *filename, int required_width, Format format);
void determine_terminal_capabilities();

int main(int argc, char **argv) {
    parse_args(argc, argv);
    determine_terminal_capabilities();

    fprintf(stderr, "Not implemented!\n");
    return -1;
}

void print_image_8(unsigned char *buffer);
void print_image_256(unsigned char *buffer);
void print_image_rgb(unsigned char *buffer);

void print_image(const char *filename, int required_width, Format format) {
    int width, height, depth;
    /* Load with 3 components. */
    unsigned char *buffer = stbi_load(filename, &width, &height, &depth, 3);

    /* Maybe do a resize. */

    /* That resized buffer? Yeah. Print it. */
    switch (format) {
        case F_8_COLOR:
            /*
            print_image_8(buffer);
            */
            break;
        case F_256_COLOR:
            /*
            print_image_256(buffer);
            */
            break;
        case F_RGB_COLOR:
            /*
            print_image_rgb(buffer);
            */
            break;
        default:
            assert(0 && "Not a valid format.");
    }

    stbi_image_free(buffer);
}

void determine_terminal_capabilities() {
    int stdout_fd = fileno(stdout);
    struct winsize ws;
    ioctl(stdout_fd, TIOCGWINSZ, &ws);
    printf("Winsize: %dx%d\n", ws.ws_col, ws.ws_row);
}

void parse_args(int argc, char **argv) {
    /* TODO: */
    options.format = F_256_COLOR;
    options.should_resize = true;
}
