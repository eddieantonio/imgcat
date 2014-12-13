/* glibc checks for this to adhere to an early 90s standard... */
#define _POSIX_C_SOURCE 2

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "print_image.h"

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
} terminal;

static void usage(FILE *dest);
static int parse_args(int argc, char **argv);
static void determine_terminal_capabilities();



int main(int argc, char **argv) {
    int width, image_name_index;

    image_name_index = parse_args(argc, argv);
    determine_terminal_capabilities();

    /* No image file specified. */
    if (image_name_index == argc) {
        fprintf(stderr, "Must specify an image!\n");
        usage(stderr);
        exit(-1);
    }

    if (options.width >= 1) {
        width = options.width;
    } else {
        width = terminal.width;
    }

    print_image(argv[image_name_index], width, F_256_COLOR);

    return 0;
}

static void determine_terminal_capabilities() {
    FILE *tput;
    int stdout_fd = fileno(stdout);
    struct winsize ws;
    int colors;

    if (!isatty(stdout_fd)) {
        terminal.isatty = false;
        return;
    }

    terminal.isatty = true;

    /* Get the current window size. */
    assert(ioctl(stdout_fd, TIOCGWINSZ, &ws) != -1);
    terminal.width = ws.ws_col;
    terminal.height = ws.ws_row;

    /* I don't really know of a better way to do this other than invoke tput. */
    assert((tput = popen("tput colors", "r")) != NULL);
    assert(fscanf(tput, "%d", &colors) == 1);
    assert(pclose(tput) != -1);
    terminal.colors = colors;
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
                options.width = (int)strtol(optarg, NULL, 10);
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
