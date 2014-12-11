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
