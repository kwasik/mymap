/*
 * rbtree.c
 *
 *  Created on: Jul 19, 2017
 *      Author: krystian
 */

#include "rbtree.h"
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/* Private macros ----------------------------------------------------------- */
#define IS_RED(node)        ((node != NULL) && (node->color == RB_RED))
#define IS_BLACK(node)      ((node != NULL) && (node->color == RB_BLACK))

#define MAX_UTF8_CHAR_SIZE  (4)

/* Private functions -------------------------------------------------------- */
int _rb_print_subtree(rb_node_t *subtree, void (print_element)(void *element),
        char *prefix, bool is_tail);
int rb_left_rotate(rb_tree_t *t, rb_node_t *node);
int rb_right_rotate(rb_tree_t *t, rb_node_t *node);

/* Exported functions ------------------------------------------------------- */
int rb_init(rb_tree_t *t, int (*compare)(rb_node_t*, rb_node_t*)) {
    if (t == NULL) return RB_NULL_PARAM;

    t->root = NULL;
    t->compare = compare;

    return RB_OK;
}

int rb_insert_fixup(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *z = node, *y;

    if (t == NULL || node == NULL) return RB_NULL_PARAM;

    /* Implementation of the insertion fix up based on "Red-Black Trees" chapter
     * from book titled "Introduction to Algorithms" */
    while (IS_RED(z->parent)) {

        if (z->parent == z->parent->parent->left) {

            /* Get uncle node */
            y = z->parent->parent->right;

            if (IS_RED(y)) {
                /* Case I: Flip colors and continue fix up at grandparent */
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
                continue;
            }

            if (z == z->parent->right) {
                /* Case II: Left rotate at parent */
                z = z->parent;
                if (rb_left_rotate(t, z) != RB_OK)
                    return RB_INTERNAL_ERR;
            }

            /* Case III: Right rotate at grandparent */
            z->parent->color = RB_BLACK;
            z->parent->parent->color = RB_RED;
            if (rb_right_rotate(t, z->parent->parent) != RB_OK)
                return RB_INTERNAL_ERR;

        } else if (z->parent == z->parent->parent->right) {

            /* Get uncle node */
            y = z->parent->parent->left;

            if (IS_RED(y)) {
                /* Case I: Flip colors and continue fix up at grandparent */
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }

            if (z == z->parent->left) {
                /* Case II: Right rotate at parent */
                z = z->parent;
                if (rb_right_rotate(t, z) != RB_OK)
                    return RB_INTERNAL_ERR;
            }

            /* Case III: Left rotate at grandparent */
            z->parent->color = RB_BLACK;
            z->parent->parent->color = RB_RED;
            if (rb_left_rotate(t, z->parent->parent) != RB_OK)
                return RB_INTERNAL_ERR;

        } else {
            /* Should never happen, but just to make sure... */
            return RB_INTERNAL_ERR;
        }
    }

    t->root->color = RB_BLACK;

    return RB_OK;
}

int rb_print_subtree(rb_node_t *subtree, void (print_element)(void *element)) {
    return _rb_print_subtree(subtree, print_element, "", false);
}

/* Private functions -------------------------------------------------------- */
int _rb_print_subtree(rb_node_t *subtree, void (print_element)(void *element),
        char *prefix, bool is_tail) {
    char *new_prefix;

    /* Allocate memory for the new prefix (we'll add up to four utf8 characters
     * and a null character) */
    new_prefix = RB_MALLOC(strlen(prefix) + 4*MAX_UTF8_CHAR_SIZE
            + sizeof(char));
    if (new_prefix == NULL) return RB_ALLOC_ERR;

    /* Print right subtree */
    strcpy(new_prefix, prefix);
    if (is_tail) {
        strcat(new_prefix, "│   ");
    } else {
        strcat(new_prefix, "    ");
    }
    if (subtree->right != NULL) {
        _rb_print_subtree(subtree->right, print_element, new_prefix, false);
    }

    /* Print current element */
    RB_PRINTF("%s", prefix);
    if (is_tail) {
        RB_PRINTF("└── ");
    } else {
        RB_PRINTF("┌── ");
    }
    switch(subtree->color) {
    case RB_RED:
        RB_PRINTF("r: ");
        break;
    case RB_BLACK:
        RB_PRINTF("b: ");
        break;
    default:
        RB_PRINTF("?: ");
        break;
    }
    if (print_element != NULL) print_element(subtree->element);
    RB_PRINTF("\n");

    /* Print left subtree */
    strcpy(new_prefix, prefix);
    if (is_tail) {
        strcat(new_prefix, "    ");
    } else {
        strcat(new_prefix, "│   ");
    }
    if (subtree->left != NULL) {
        _rb_print_subtree(subtree->left, print_element, new_prefix, true);
    }

    RB_FREE(new_prefix);

    return RB_OK;
}

int rb_left_rotate(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *x = node, *y = node->right;

    /* TODO: Check pointers for null before dereferencing */

    /*
     * Rotate left:
     *
     *    |              |
     *    x              y
     *   / \            / \
     *  A   y    ->    x   C
     *     / \        / \
     *    B   C      A   B
     */

    x->right = y->left;
    if (y->left != NULL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL) {
        t->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;

    return RB_OK;
}

int rb_right_rotate(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *x = node, *y = node->left;

    /* TODO: Check pointers for null before dereferencing */

    /*
     * Rotate right:
     *
     *      |          |
     *      x          y
     *     / \        / \
     *    y   C  ->  A   x
     *   / \            / \
     *  A   B          B   C
     */

    x->left = y->right;
    if (y->right != NULL) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL) {
        t->root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;

    return RB_OK;
}
