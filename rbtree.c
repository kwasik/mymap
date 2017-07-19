/*
 * rbtree.c
 *
 *  Created on: Jul 19, 2017
 *      Author: krystian
 */

#include "rbtree.h"
#include <stdbool.h>

/* Private macros ----------------------------------------------------------- */
#define IS_RED(node)        ((node != NULL) && (node->color == RB_RED))
#define IS_BLACK(node)      ((node != NULL) && (node->color == RB_BLACK))

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

    t->root = RB_BLACK;

    return RB_OK;
}

int rb_print_subtree(rb_node_t *subtree, void (print_element)(void *element)) {
    return _rb_print_subtree(subtree, print_element, "", false);
}

/* Private functions -------------------------------------------------------- */
int _rb_print_subtree(rb_node_t *subtree, void (print_element)(void *element),
        char *prefix, bool is_tail) {
    char *new_prefix;
    size_t prefix_length = strlen(prefix);

    /* Allocate memory for new prefix and copy the old part */
    new_prefix = malloc(strlen(prefix) + 5);
    if (new_prefix == NULL) return RB_ALLOC_ERR;
    strcpy(new_prefix, prefix);

    /* Print right subtree */
    if (is_tail) {
        strcpy(new_prefix + prefix_length, "│   ");
    } else {
        strcpy(new_prefix + prefix_length, "    ");
    }
    if (subtree->right != NULL) {
        _rb_print_subtree(subtree->right, new_prefix, false);
    }

    /* Print current element */
    printf("%s", prefix);
    if (is_tail) {
        printf("└── ");
    } else {
        printf("┌── ");
    }
    if (print_element != NULL) print_element(subtree->element);
    printf("\n");

    /* Print left subtree */
    if (is_tail) {
        strcpy(new_prefix + prefix_length, "    ");
    } else {
        strcpy(new_prefix + prefix_length, "│   ");
    }
    if (subtree->left != NULL) {
        _rb_print_subtree(subtree->left, new_prefix, true);
    }

    free(new_prefix);

    return RB_OK;
}

int rb_left_rotate(rb_tree_t *t, rb_node_t *node) {
    return RB_ERR;
}

int rb_right_rotate(rb_tree_t *t, rb_node_t *node) {
    return RB_ERR;
}
