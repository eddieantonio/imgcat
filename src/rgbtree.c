/*
 * Copyright (c) 2012, 2014, 2017, Eddie Antonio Santos <easantos@ualberta.ca>
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
 * @file rgbtree.c
 * @brief Implementation of RGB k-d tree things and stuff.
 * @author Eddie Antonio Santos <easantos@ualberta.ca>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "rgbtree.h"

#include "tree_256_color.h"
static const RGB_Node *sample_tree = TREE_256_COLOR;


/*************
 * Functions *
 *************/


/* Nearest neighbour functions. */
typedef struct result {
    long            distance_squared;
    const RGB_Node  *node;
} RGB_Result;

static void find_nearest(const RGB_Node *tree, RGB_Tuple *target,
        RGB_Result *current_best)
{

    int axis = tree->axis;
    RGB_Node *nearer_subtree, *farther_subtree;

    /* Figure out which side of the splitting plane is the nearest. */
    if (target->axis[axis] < tree->colour.axis[axis]) {
        nearer_subtree = tree->left;
        farther_subtree = tree->right;
    } else {
        nearer_subtree = tree->right;
        farther_subtree = tree->left;
    }

    if (nearer_subtree) {
        find_nearest(nearer_subtree, target, current_best);
    }

    long this_distance = rgb_colour_distance(target, &tree->colour);

    if (this_distance < current_best->distance_squared) {
        current_best->node = tree;
        current_best->distance_squared = this_distance;
    }

    if (farther_subtree) {
        long radius = labs(target->axis[axis] - tree->colour.axis[axis]);
        if (radius < current_best->distance_squared) {
            find_nearest(farther_subtree, target, current_best);
        }
    }

}

const RGB_Node *rgb_nearest(const RGB_Node *root, RGB_Tuple *target,
        long *distance_loc) {

    RGB_Result result;

    /* Initially, guess that the root is the closest. */
    result.node = root;
    result.distance_squared = rgb_colour_distance(&root->colour, target);

    /* Go calculate everything. */
    find_nearest(root, target, &result);

    if (distance_loc) {
        *distance_loc = result.distance_squared;
    }

    return result.node;
}

const RGB_Node *rgb_closest_colour(uint8_t red, uint8_t green, uint8_t blue) {

    const RGB_Node *root, *closest;
    RGB_Tuple target_colour;
    long distance = -1;

    root = &sample_tree[0];
    target_colour.channel.red = red;
    target_colour.channel.green = green;
    target_colour.channel.blue = blue;

    closest = rgb_nearest(root, &target_colour, &distance);

    return closest;

}

/* Traversal functions. */
static void foreach_df(const RGB_Node *node, NodeFunc func, int depth) {
    func(node, depth);

    if (node->left) {
        foreach_df(node->left, func, depth + 1);
    }

    if (node->right) {
        foreach_df(node->right, func, depth + 1);
    }

}

void rgb_foreach_df(const RGB_Node *node,
        void (*func)(const RGB_Node *, int)) {
    foreach_df(node, func, 0);
}

/* Traversing callbacks. */
void rgb_print_node(const RGB_Node *node, int depth) {
    const RGB_Tuple *rgb = &node->colour;

    /* Print indents for depth. */
    int i;
    for (i = 0; i < depth; i++) {
        putchar('\t');
    }

    printf("%03d (0x%02X%02X%02X)\n", node->id,
            (int) rgb->channel.red,
            (int) rgb->channel.green,
            (int) rgb->channel.blue);

}

/* Max distance is (255**2 + 255**2 + 255**2) == 195,075 */
_Static_assert(195075L <= INT_MAX, "Cannot store max distance in UINT");

/* Utility functions. */
int rgb_colour_distance(const RGB_Tuple *p, const RGB_Tuple *q) {
    int i;
    int accumulator, squared_distance = 0;

    for (i = 0; i < NUM_CHANNELS; i++) {
        accumulator = p->axis[i]- q->axis[i];
        accumulator *= accumulator;
        squared_distance += accumulator;
    }

    return squared_distance;
}
