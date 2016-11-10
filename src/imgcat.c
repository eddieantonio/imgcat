/* glibc checks for this to adhere to an early 90s standard... */
#define _POSIX_C_SOURCE 2

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "print_image.h"

#define VERSION     "1.1.1"
#define EXIT_USAGE  -1


/* Global, bite me. */
static struct {
    Format format;
    bool should_resize;
    int width;
} options = {
    .format = F_UNSET,          /* Default: 256 colors. */
    .should_resize = true,      /* Default: yes! */
    .width = WIDTH_UNSET,
};

/* Terminal info. */
static struct {
    int width;
    int height;
    int colors;
    bool isatty;
    Format optimum_format;
} terminal;

/* Long options */
static struct option long_options[] = {
     { "no-resize",     no_argument,            NULL,           'R' },
     { "width",         required_argument,      NULL,           'w' },
     { "depth",         required_argument,      NULL,           'd' },
     { "help",          no_argument,            NULL,           'h' },
     { "version",       no_argument,            NULL,           'v' },
     { NULL,            0,                      NULL,           0 }
};


/* Returns index of first positional argument. */
static int parse_args(int argc, char **argv);
static void bad_usage(const char *msg, ...) __attribute__((noreturn));
static void determine_terminal_capabilities();
static void usage(FILE *dest);

/* Set first thing in main(). */
static char const* program_name;


int main(int argc, char **argv) {
    int width, image_name_index;
    bool status;
    const char *image_name;
    Format color_format = F_UNSET;
    program_name = argv[0];

    image_name_index = parse_args(argc, argv);

    /* No image file specified. */
    if (image_name_index == argc) {
        bad_usage("Must specify an image file.");
    }

    determine_terminal_capabilities();

    /* Determine if the image should be resized. */
    if ((options.width >= 1) && (options.should_resize)) {
        /* Use explicitly provided width. */
        width = options.width;
    } else if ((!terminal.isatty) || (!options.should_resize)) {
        /* Don't resize if not a terminal or told to do so. */
        width = WIDTH_UNSET;
    } else {
        /* Use maximum width from what was infered of the terminal. */
        width = terminal.width;
    }

    /* Set color format either from options, or infered from the terminal. */
    if (options.format != F_UNSET) {
        color_format = options.format;
    } else {
        color_format = terminal.optimum_format;
    }

    image_name = argv[image_name_index];

    status = print_image(image_name, width, color_format);

    if (!status) {
        bad_usage("Failed to open image: %s", image_name);
    }

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

    if (colors >= 256) {
        terminal.optimum_format = F_256_COLOR;
    } else if (colors >= 8) {
        terminal.optimum_format = F_8_COLOR;
    } else {
        assert(0 && "Don't know what color depth is best for you...");
    }
}


static void usage(FILE *dest) {
    fprintf(dest, "usage:\timgcat [--width WIDTH|--no-rescale]"
                                " [--depth (8|256)] IMAGE\n");
}

static void bad_usage(const char *msg, ...) {
    va_list args;

    fprintf(stderr, "%s: ", program_name);

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);

    fprintf(stderr, "\n");

    usage(stderr);

    exit(EXIT_USAGE);
}

static Format parse_format(const char *arg) {
    if (strncmp(arg, "256", 4) == 0) {
        return F_256_COLOR;
    } else if ((strncmp(arg, "8", 2) == 0) || (strncmp(arg, "ansi", 5) == 0)) {
        return F_8_COLOR;
    } else if (strncmp(arg, "rgb", 4) == 0) {
        return F_UNSET;
    }
    return F_UNSET;
}

static int parse_args(int argc, char **argv) {
    int c;

    while (1) {
        c = getopt_long(argc, argv, "w:d:Rhv", long_options, NULL);
        if (c == -1)
            break;

        switch (c) {
            case 'w':
                options.width = (int)strtol(optarg, NULL, 10);
                if (options.width < 1) {
                    bad_usage("Width must be a positive integer, not '%s'",
                              optarg);
                }
                break;

            case 'd':
                options.format = parse_format(optarg);
                if (options.format == F_UNSET) {
                    bad_usage("Unknown output format: %s", optarg);
                }
                break;
            case 'R':
                options.should_resize = false;
                break;

            case 'h':
                usage(stdout);
                exit(0);
                break;

            case 'v':
                printf("%s %s\n", program_name, VERSION);
                exit(0);
                break;

            /* Unknown option. */
            case '?':
            default:
                usage(stderr);
                exit(-1);
        }
    }

    return optind;
}
