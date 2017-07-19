/*
 * rbtree.c
 *
 *  Created on: Jul 19, 2017
 *      Author: krystian
 */

#include "rbtree.h"
#include <stdbool.h>

/* Private macros ------------------------------------------------------------------------------- */
#define IS_RED(node)        ((node != NULL) && (node->color == RB_RED))
#define IS_BLACK(node)      ((node != NULL) && (node->color == RB_BLACK))

/* Exported functions --------------------------------------------------------------------------- */
int rb_init(rb_tree_t *t, int (*compare)(rb_node_t*, rb_node_t*)) {
    if (t == NULL) return RB_NULL_PTR;

    t->root = NULL;
    t->compare = compare;

    return RB_OK;
}

int rb_insert_fixup(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *z = node, *y;

    if (t == NULL || node == NULL) return RB_NULL_PTR;

    /* Implementation of the insertion fix up based on "Red-Black Trees" chapter from book titled
     * "Introduction to Algorithms" */
    while (IS_RED(z->parent)) {

        if (z->parent == z->parent->parent->left) {

            /* Get uncle node */
            y = z->parent->parent->right;

            if (IS_RED(y)) {
                /* Case I: Flip colors and continue fix up at grandparent node */
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
                continue;
            }

            if (z == z->parent->right) {
                /* Case II: Left rotate at parent */
                z = z->parent;
                rb_left_rotate(t, z);
            }

            /* Case III: Right rotate at grandparent */
            z->parent->color = RB_BLACK;
            z->parent->parent->color = RB_RED;
            rb_right_rotate(t, z->parent->parent);

        } else if (z->parent == z->parent->parent->right) {

            /* Get uncle node */
            y = z->parent->parent->left;

            if (IS_RED(y)) {
                /* Case I: Flip colors and continue fix up at grandparent node */
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }

            if (z == z->parent->left) {
                /* Case II: Right rotate at parent */
                z = z->parent;
                rb_right_rotate(t, z);
            }

            /* Case III: Left rotate at grandparent */
            z->parent->color = RB_BLACK;
            z->parent->parent->color = RB_RED;
            rb_left_rotate(t, z->parent->parent);

        } else {
            /* Should never happen, but just to make sure... */
            return RB_INTERNAL_ERR;
        }
    }

    t->root = RB_BLACK;

    return RB_OK;
}
