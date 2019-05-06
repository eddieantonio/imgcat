/*
 * Copyright (c) 2014–2018, Eddie Antonio Santos <easantos@ualberta.ca>
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

/* Feature-test macro for fileno(3), fdopen(3), and mkstemp(3). */
#define _XOPEN_SOURCE 500
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <err.h>

#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <sysexits.h>

#include <term.h>

#include "print_image.h"
#include "config.h"

/* All the information I care about the terminal. */
struct terminal_t {
    int width;
    int height;
    int colors;
    bool isatty;
    Format optimum_format;
};

/**
 * Global containing all relevant command line options.
 */
static struct {
    Format format;
    bool should_resize;
    int width;
    int height;
    bool use_half_height;
    bool use_fake_terminal;
} options = {
    .format = F_UNSET,          /* Default: autodetect highest fidelity. */
    .should_resize = true,      /* Default: yes! */
    .width = WIDTH_UNSET,
    .height = HEIGHT_UNSET,
    .use_half_height = false,
    .use_fake_terminal = false,
};

/**
 * Info about the user's real terminal.
 */
static struct terminal_t real_terminal = {
    .width = WIDTH_UNSET,
    .height = HEIGHT_UNSET,
    .colors = 0,
    .isatty = false,
    .optimum_format = F_8_COLOR
};

/**
 * Fake terminal used in --x-terminal-override.
 */
static struct terminal_t fake_terminal = { 0 };

/* Global temporary filename for dumping stdin into.  */
#define MAX_TEMPFILE_NAME 128
static char tempfile_name[MAX_TEMPFILE_NAME + 1];

/* Long options */
static struct option long_options[] = {
    /* Options affecting output colour depth. */
    { "depth",          required_argument,      NULL,           'd' },

    /* Options affecting size. */
    { "no-resize",      no_argument,            NULL,           'R' },
    { "width",          required_argument,      NULL,           'w' },
    { "height",         required_argument,      NULL,           'r' },
    { "half-height",    no_argument,            NULL,           'H' },

    /* Abbreviated options. */
    { "8",      no_argument, (int*) &options.format,    F_8_COLOR   },
    { "ansi",   no_argument, (int*) &options.format,    F_8_COLOR   },
    { "256",    no_argument, (int*) &options.format,    F_256_COLOR },
    { "iterm2", no_argument, (int*) &options.format,    F_ITERM2    },

    /* Common options. */
    { "help",           no_argument,            NULL,           'h' },
    { "version",        no_argument,            NULL,           'v' },

    /* Options for internal use and debugging. */
    /* These flags are EXPLICITLY undocumented, as they are for development
     * use only, and can change or be removed at any time. */
    { "x-terminal-override", required_argument, NULL,           'x' },

    { NULL,             0,                      NULL,           0   }
};


/** Returns the filename of the image to open. */
static const char* parse_args(int argc, char **argv);
static void bad_usage(const char *msg, ...) __attribute__((noreturn));
static void fatal_error(int code, const char *msg, ...) __attribute__((noreturn));
static void determine_terminal_capabilities();
static void determine_optimum_color_format(struct terminal_t *);
static void set_fake_terminal(const char *);
static void usage(FILE *dest);
static const char *dump_stdin_into_tempfile();

/* Set first thing in main(). */
static char const* program_name;


int main(int argc, char **argv) {
    int desired_width = WIDTH_UNSET;
    int desired_height = HEIGHT_UNSET;
    bool status;
    const char *image_name;
    Format color_format = F_UNSET;
    struct terminal_t* terminal;
    program_name = argv[0];

    image_name = parse_args(argc, argv);
    if (image_name == NULL) {
        if (isatty(fileno(stdin))) {
            /* No image is specified on the command line, and there's nothing
             * redirected to stdin. */
            bad_usage("Must specify an image file.");
        } else {
            /* There's an image redirected to stdin. */
            image_name = dump_stdin_into_tempfile();
        }
    }

    /* Determine whether to use the real termainal or the fake (overridden)
     * terminal */
    if (options.use_fake_terminal) {
        /* For debugging and testing, use the overridden terminal. */
        terminal = &fake_terminal;
        assert(terminal->isatty);
        fprintf(stderr, "Using overridden terminal: %dx%d at %d colors\n",
                terminal->width, terminal->height, terminal->colors);
    } else {
        determine_terminal_capabilities();
        terminal = &real_terminal;
    }

    /* Determine if the image should be resized. */
    if (options.should_resize) {
        /* Use explicitly provided dimensions. */
        if (options.width != WIDTH_UNSET) {
            desired_width = options.width;
        }
        if (options.height != HEIGHT_UNSET) {
            desired_height = options.height;
        }
    }

    /* Set color format either from options, or infered from the terminal. */
    if (options.format != F_UNSET) {
        color_format = options.format;
    } else {
        color_format = terminal->optimum_format;
    }

    PrintRequest request = (PrintRequest) {
        .filename = image_name,
        .desired_width = desired_width,
        .desired_height = desired_height,
        .max_width = terminal->width,
        .max_height = terminal->height,
        .half_height = options.use_half_height,
        .format = color_format
    };
    status = print_image(&request);

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

    real_terminal.isatty = true;

    /* Get the current window size. */
    assert(ioctl(stdout_fd, TIOCGWINSZ, &ws) != -1);
    real_terminal.width = ws.ws_col;
    real_terminal.height = ws.ws_row;

    real_terminal.colors = get_terminal_colours();

    /* ITERM_SESSION_ID is exported in iTerm2 sessions. */
    if (getenv("ITERM_SESSION_ID") != NULL) {
        real_terminal.optimum_format = F_ITERM2;
    } else {
        /* Otherwise, determine the capability from the reported colours. */
        determine_optimum_color_format(&real_terminal);
    }
}

/**
 * Figures out the ideal color format for a terminal, by only considering its
 * self-reported color depth.
 */
static void determine_optimum_color_format(struct terminal_t *terminal) {
    if (terminal->colors >= 256) {
        terminal->optimum_format = F_256_COLOR;
    } else if (terminal->colors >= 8) {
        terminal->optimum_format = F_8_COLOR;
    } else {
        fprintf(stderr, "%s: Cannot determine optimum color depth for "
                "reported %d terminal colors setting\n",
                program_name, terminal->colors);
    }
}

/**
 * Removes the temporary file. Intended to be the atexit() callback.
 */
static void unlink_tempfile(void) {
    remove(tempfile_name);
}

/**
 * This is necessary because CImg likes to close and reopen the file it's
 * reading, which discards header data when reading from /dev/stdin.
 */
static const char *dump_stdin_into_tempfile() {
    int byte;
    /* Set up the mutable buffer for mkstemp() to do its magic. */
    strncpy(tempfile_name, "/tmp/image.XXXXXXXX", MAX_TEMPFILE_NAME);
    int output_fd = mkstemp(tempfile_name);
    if (output_fd == -1) {
        fatal_error(EX_TEMPFAIL, "could not create temporary file: %s",
                    strerror(errno));
    }

    FILE *output = fdopen(output_fd, "wb");
    if (output == NULL) {
        fatal_error(EX_IOERR, "could not open temporary file: %s",
                    strerror(errno));
    }

    /* TODO: do something better than a byte-for-byte file transfer. */
    while ((byte = getchar()) != EOF) {
        fputc(byte, output);
    }
    fclose(output);

    /* Remove the file at ordinary exit. */
    atexit(unlink_tempfile);

    return tempfile_name;
}

static void usage(FILE *dest) {
    const int field_width = strlen(program_name);
    fprintf(dest, "Usage:\n");
    fprintf(dest,
            "\t%s"  " [--width=<columns> --height=<rows>|--no-resize]\n"
            "\t%*c" " [--half-height] [--depth=(8|256|iterm2)] IMAGE\n",
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

static void fatal_error(int code, const char *msg, ...) {
    va_list args;

    fprintf(stderr, "%s: ", program_name);

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");

    exit(code);
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
        c = getopt_long(argc, argv, "w:r:d:RHhv", long_options, NULL);
        if (c == -1) {
            break;
        }

        switch (c) {
            case 'w': /* --width */
                options.width = (int)strtol(optarg, NULL, 10);
                if (options.width < 1) {
                    bad_usage("Width must be a positive integer, not '%s'",
                              optarg);
                }
                break;

            case 'r': /* --rows */
                options.height = (int)strtol(optarg, NULL, 10);
                if (options.height < 1) {
                    bad_usage("Height must be a positive integer, not '%s'",
                              optarg);
                }
                break;

            case 'd': /* --depth=(8|ansi|256|iterm2) */
                options.format = parse_format(optarg);
                if (options.format == F_UNSET) {
                    bad_usage("Unknown output format: %s", optarg);
                }
                break;

            case 'R': /* --no-resize */
                options.should_resize = false;
                break;

            case 'H': /* --half-height */
                options.use_half_height = true;
                break;

            case 'h': /* --help */
                usage(stdout);
                exit(EXIT_SUCCESS);
                break;

            case 'v': /* --version */
                printf("%s %s\n", program_name, PACKAGE_VERSION);
                exit(EXIT_SUCCESS);
                break;

            case 'x': /* --x-terminal-override */
                options.use_fake_terminal = true;
                set_fake_terminal(optarg);
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
        return NULL;
    } else {
        return argv[optind];
    }
}


/**
 * Parses the --x-terminal-override string.
 *
 * Options are: WIDTHxHEIGHTxCOLORS
 */
static void set_fake_terminal(const char *override_string) {
    int width, height, colors;

    int opts = sscanf(override_string, "%dx%d:%d", &width, &height, &colors);
    if (opts != 3) {
        bad_usage("invalid override string: %s", override_string);
    }

    if (width <= 0 || height <= 0 || colors <= 0) {
        bad_usage("invalid settings: %s", override_string);
    }

    fake_terminal.width = width;
    fake_terminal.height = height;
    fake_terminal.colors = colors;
    determine_optimum_color_format(&fake_terminal);
    /* We're doing this --x-terminal-override stuff to *simulate* isatty
     * without calling it, so ALWAYS set isatty to true. */
    fake_terminal.isatty = true;
}
