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

/**
 * @file rgbtree.h
 * @brief Describes a kd-tree data structure for finding colour names.
 * @author Eddie Antonio Santos <easantos@ualberta.ca>
 */

#ifndef RGBTREE_H
#define RGBTREE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum rgb_channels {
    RED, GREEN, BLUE,
    /* Sentinel/length value: */
    NUM_CHANNELS
} Channel;

typedef union {
    uint8_t     axis[3];
    struct {
        uint8_t red, green, blue;
    } channel;
} RGB_Tuple;

/* Forward declaration. */
typedef struct kd_node RGB_Node;
struct kd_node {
    uint8_t     id;
    RGB_Tuple   colour;
    Channel     axis;
    RGB_Node    *left, *right;
};

/* Used in rgb_foreach_df. */
typedef void (*NodeFunc)(const RGB_Node*, int);

const RGB_Node *rgb_nearest(const RGB_Node *tree, RGB_Tuple *target, long *dist);
void rgb_foreach_df(const RGB_Node *tree, void (*func)(const RGB_Node*, int));
int rgb_colour_distance(const RGB_Tuple *p, const RGB_Tuple *q);
const RGB_Node *rgb_closest_colour(uint8_t red, uint8_t green, uint8_t blue);
void rgb_print_node(const RGB_Node *node, int depth);

#endif /* RGBTREE_H */
