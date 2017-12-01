/*
 * Copyright (c) 2014, 2017, Eddie Antonio Santos <easantos@ualberta.ca>
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

/* Feature-test macro for fileno(3). */
#define _XOPEN_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sysexits.h>

#include <term.h>

#include "print_image.h"
#include "config.h"


/**
 * Global containing all relevant command line options.
 */
static struct {
    Format format;
    bool should_resize;
    int width;
} options = {
    .format = F_UNSET,          /* Default: autodetect highest fidelity. */
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
} terminal = {
    .width = WIDTH_UNSET,
    .height = 0,
    .colors = 0,
    .isatty = false,
    .optimum_format = F_8_COLOR
};

/* Long options */
static struct option long_options[] = {
    /* Options affecting output colour depth. */
    { "depth",          required_argument,      NULL,           'd' },

    /* Options affecting resizing. */
    { "no-resize",      no_argument,            NULL,           'R' },
    { "width",          required_argument,      NULL,           'w' },


    /* Abbreviated options. */
    { "8",      no_argument, (int*) &options.format,    F_8_COLOR   },
    { "ansi",   no_argument, (int*) &options.format,    F_8_COLOR   },
    { "256",    no_argument, (int*) &options.format,    F_256_COLOR },
    { "iterm2", no_argument, (int*) &options.format,    F_ITERM2    },

    /* Common options. */
    { "help",           no_argument,            NULL,           'h' },
    { "version",        no_argument,            NULL,           'v' },

    { NULL,             0,                      NULL,           0   }
};


/* Returns index of first positional argument. */
static const char* parse_args(int argc, char **argv);
static void bad_usage(const char *msg, ...) __attribute__((noreturn));
static void determine_terminal_capabilities();
static void usage(FILE *dest);

/* Set first thing in main(). */
static char const* program_name;


int main(int argc, char **argv) {
    int width;
    bool status;
    const char *image_name;
    Format color_format = F_UNSET;
    program_name = argv[0];

    image_name = parse_args(argc, argv);

    /* No image file specified. */
    /* TODO: no image specified on stdin file line. */
    /* bad_usage("Must specify an image file."); */

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

    status = print_image(image_name, width, color_format);

    if (!status) {
        bad_usage("Failed to open image: %s", image_name);
    }

    return EXIT_SUCCESS;
}

/**
 * Get the color capability from the terminfo database.
 */
static int get_terminal_colours() {
    char tbuf[1024];
    if (tgetent(tbuf, getenv("TERM")) != 1) {
        return -1;
    }

    return tigetnum("colors");
}

/**
 * Determines the terminal's capabilities:
 * its optimum colour depth and dimensions.
 */
static void determine_terminal_capabilities() {
    int stdout_fd = fileno(stdout);
    struct winsize ws;

    /* If stdout is NOT a terminal (it's redirected to a file perhaps),
     * just bail. */
    if (!isatty(stdout_fd)) {
        return;
    }

    terminal.isatty = true;

    /* Get the current window size. */
    assert(ioctl(stdout_fd, TIOCGWINSZ, &ws) != -1);
    terminal.width = ws.ws_col;
    terminal.height = ws.ws_row;

    terminal.colors = get_terminal_colours();

    /* ITERM_SESSION_ID is exported in iTerm2 sessions. */
    if (getenv("ITERM_SESSION_ID") != NULL) {
        terminal.optimum_format = F_ITERM2;
    /* Otherwise, determine the capability from the reported colours. */
    } else if (terminal.colors >= 256) {
        terminal.optimum_format = F_256_COLOR;
    } else if (terminal.colors >= 8) {
        terminal.optimum_format = F_8_COLOR;
    } else {
        assert(0 && "Don't know what color depth is best for you...");
    }
}

enum {
    MAX_TEMPFILE_NAME = 128
};
static char tempfile_name[MAX_TEMPFILE_NAME + 1];

/**
 * This is necessary because CImg likes to close and reopen the file it's
 * reading, which discards header data when reading from stdin.
 */
static const char *dump_stdin_into_tempfile() {
    int byte;
    strncpy(tempfile_name, "/tmp/image.XXXXXXXX", MAX_TEMPFILE_NAME);
    /* TODO: null check. */
    mktemp(tempfile_name);
    FILE *output = fopen(tempfile_name, "wb");

    /* TODO: do something better than a byte-for-byte file transfer. */
    while ((byte = getchar()) != EOF) {
        fputc(byte, output);
    }
    fclose(output);

    /* TODO: setup atexit hook to unlink the file? */
    return tempfile_name;
}

static void usage(FILE *dest) {
    const int field_width = strlen(program_name);
    fprintf(dest, "Usage:\n");
    fprintf(dest,
            "\t%s"  " [--width=<characters>|--no-rescale]\n"
            "\t%*c" " [--depth=(8|256|iterm2)] IMAGE\n",
            program_name, field_width, ' ');
    fprintf(dest, "\t"
            "%s --version\n", program_name);
    fprintf(dest, "\t"
            "%s --help\n", program_name);
}

static void bad_usage(const char *msg, ...) {
    va_list args;

    fprintf(stderr, "%s: ", program_name);

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");

    usage(stderr);

    exit(EX_USAGE);
}


static Format parse_format(const char *arg) {
#   define argeq(b)     (strncmp(arg, (b), (sizeof(b))) == 0)

    if (argeq("256")) {
        return F_256_COLOR;
    } else if (argeq("8") || argeq("ansi")) {
        return F_8_COLOR;
    } else if (argeq("iterm2")) {
        return F_ITERM2;
    }

    return F_UNSET;
#   undef argeq
}

static const char* parse_args(int argc, char **argv) {
    int c;
    /* Disable getopt_long from printing to stderr. */
    extern int opterr;
    opterr = 0;

    while (1) {
        c = getopt_long(argc, argv, "w:d:Rhv", long_options, NULL);
        if (c == -1) {
            break;
        }

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
                exit(EXIT_SUCCESS);
                break;

            case 'v':
                printf("%s %s\n", program_name, PACKAGE_VERSION);
                exit(EXIT_SUCCESS);
                break;

            case 0:
                /* Set an abbreviated option like --8, --ansi, --256. */
                break;

            /* Unknown option. */
            case '?':
            default:
                bad_usage("Unknown option: %s", argv[optind - 1]);
        }
    }

    if (argc == optind) {
        /* This means getopt() has exhausted all of argv and has not found a
         * valid image name argument. */
        return dump_stdin_into_tempfile();
    } else {
        return argv[optind];
    }
}
