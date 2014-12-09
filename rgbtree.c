/**
 * @file rgbtree.c
 * @brief Implementation of RGB k-d tree things and stuff.
 * @author Eddie Antonio Santos <easantos@ualberta.ca>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rgbtree.h"

/*#include "generated_rgb_tree.h"*/
static RGB_Node sample_tree[] = {
    { .name = "Seafoam",    .colour = {{63, 223, 191}},     .axis = RED,
        .left = &sample_tree[1], .right = &sample_tree[4] },

    { .name = "DodgerBlue", .colour = {{31, 127, 255}},     .axis = GREEN,
        .left = &sample_tree[2], .right = &sample_tree[3] },
    { .name = "Black",      .colour = {{0, 0, 0}},          .axis = BLUE,
        .left = 0, .right = 0 },
    { .name = "Off-Green",  .colour = {{0, 223, 0 }},       .axis = BLUE,
        .left = 0, .right = 0 },

    { .name = "Pink",       .colour = {{255, 191, 223}},    .axis = GREEN,
        .left = &sample_tree[5], .right = &sample_tree[6] },
    { .name = "Gold",       .colour = {{255, 223, 0}},      .axis = BLUE,
        .left = 0, .right = 0 },
    { .name = "Crimson",    .colour = {{223, 0, 0}},        .axis = BLUE,
        .left = 0, .right = 0 },

};


/*************
 * Functions *
 *************/


/* Nearest neighbour functions. */
typedef struct result {
    long        distance_squared;
    RGB_Node    *node;
} RGB_Result;

static void find_nearest(RGB_Node *tree, RGB_Tuple *target,
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

RGB_Node *rgb_nearest(RGB_Node *root, RGB_Tuple *target, long *distance_loc) {

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

RGB_Node *rgb_closest_colour(uint8_t red, uint8_t green, uint8_t blue) {

    RGB_Node *root, *closest;
    RGB_Tuple target_colour;
    long distance = -1;

    root = &sample_tree[0];
    target_colour.channel.red = red;
    target_colour.channel.green = green;
    target_colour.channel.blue = blue;

    closest = rgb_nearest(root, &target_colour, &distance);

    return closest;

}

/* Construction */
#define SORT_TEMPLATE(name, index) \
    static int sort_##name (const void *a, const void *b) { \
        RGB_Node *node_a = (RGB_Node*) a; \
        RGB_Node *node_b = (RGB_Node*) b; \
        return node_a->colour.axis[index] - node_b->colour.axis[index]; \
    }

typedef int (*ComparisonFunc)(const void *, const void*);
SORT_TEMPLATE(red, 0)
SORT_TEMPLATE(green, 1)
SORT_TEMPLATE(blue, 2)
static ComparisonFunc comparisons[] = {
    sort_red, sort_green, sort_blue
};

#define CHANNELS (sizeof(comparisons)/sizeof(ComparisonFunc))

RGB_Node *rgb_construct(RGB_Node *nodes, int count, int depth) {
    /* `unsorted_nodes` should be a big array of RGB_Node structs. The
     * name and the colour should be set to whatever they will be in the
     * final structure, but the initial value for the axis *MUST* be 0.
     * The left and right pointers can be anything, (though they should
     * probably be 0 as well).
     */

    /* Step 0: Base case: No count, no tree. */
    if (count < 1) {
        return NULL;
    }

    int axis = depth % CHANNELS;
    /* Step 1: Sort! */
    qsort(nodes, count, sizeof(RGB_Node), comparisons[axis]);

    /* Step 2: Get the median. */
    int median_index = count / 2;
    RGB_Node *median = &nodes[median_index];
    median->axis = axis;
    
    /* Step 3: Build tree recursively. */
    RGB_Node *lesser, *greater;
    int lesser_count, greater_count;

    lesser = nodes;
    lesser_count = count / 2;
    median->left = rgb_construct(lesser, lesser_count, depth + 1);

    greater = &nodes[median_index + 1];
    greater_count = (count - 1) /2;
    median->right = rgb_construct(greater, greater_count, depth + 1);

    return median;
}


/* Traversal functions. */
static void foreach_df(RGB_Node *node, void (*func)(RGB_Node*, int), int
        depth)
{
    func(node, depth);

    if (node->left) {
        foreach_df(node->left, func, depth + 1);
    }

    if (node->right) {
        foreach_df(node->right, func, depth + 1);
    }

}

void rgb_foreach_df(RGB_Node *node, void (*func)(RGB_Node *, int)) {
    foreach_df(node, func, 0);
}

/* Traversing callbacks. */
void rgb_print_node(RGB_Node *node, int depth) {
    if (!node->name) {
        printf("Null colour node!\n");
        return;
    }

    RGB_Tuple *rgb = &node->colour;

    /* Print indents for depth. */
    int i;
    for (i = 0; i < depth; i++) {
        putchar('\t');
    }

    printf("%s (0x%02X%02X%02X)\n", node->name,
            (int) rgb->channel.red,
            (int) rgb->channel.green,
            (int) rgb->channel.blue);

}

/* Utility functions. */
unsigned long rgb_colour_distance(RGB_Tuple *p, RGB_Tuple *q) {
    int i;
    unsigned long accumulator, squared_distance = 0;

    for (i = 0; i < NUM_CHANNELS; i++) {
        accumulator = p->axis[i]- q->axis[i];
        accumulator *= accumulator;
        squared_distance += accumulator;
    }

    return squared_distance;
}

