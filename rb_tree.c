/*
 * rbtree.c
 *
 *  Created on: Jul 19, 2017
 *      Author: krystian
 */

#include "rb_tree.h"
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

/* Private macros ----------------------------------------------------------- */
#define IS_RED(node)        ((node != NULL) && (node->color == RB_RED))
#define IS_BLACK(node)      ((node != NULL) && (node->color == RB_BLACK))

#define MAX_UTF8_CHAR_SIZE  (4)

/* Private functions -------------------------------------------------------- */
static int _rb_print_subtree(rb_node_t *subtree,
        void (print_element)(void *element), char *prefix, bool is_tail);
static int rb_left_rotate(rb_tree_t *t, rb_node_t *node);
static int rb_right_rotate(rb_tree_t *t, rb_node_t *node);
static int rb_transplant(rb_tree_t *t, rb_node_t *u, rb_node_t *v);
static rb_node_t* rb_minimum(rb_node_t *node);

/* Exported functions ------------------------------------------------------- */
int rb_init(rb_tree_t *t) {
    if (t == NULL) return RB_NULL_PARAM;

    t->root = NULL;

    return RB_OK;
}

int rb_insert_fixup(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *z = node, *y;

    if (t == NULL || node == NULL) return RB_NULL_PARAM;

    /* All tree manipulations were implemented based on "Red-Black Trees"
     * chapter from "Introduction to Algorithms". */

    while (IS_RED(z->parent)) {

        if (z->parent == z->parent->parent->left) {

            /* Get uncle node */
            y = z->parent->parent->right;

            if (IS_RED(y)) {
                /* Case I: */
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
                continue;
            }

            if (z == z->parent->right) {
                /* Case II: */
                z = z->parent;
                if (rb_left_rotate(t, z) != RB_OK)
                    return RB_INTERNAL_ERR;
            }

            /* Case III: */
            z->parent->color = RB_BLACK;
            z->parent->parent->color = RB_RED;
            if (rb_right_rotate(t, z->parent->parent) != RB_OK)
                return RB_INTERNAL_ERR;

        } else if (z->parent == z->parent->parent->right) {

            /* Get uncle node */
            y = z->parent->parent->left;

            if (IS_RED(y)) {
                /* Case I: */
                z->parent->color = RB_BLACK;
                y->color = RB_BLACK;
                z->parent->parent->color = RB_RED;
                z = z->parent->parent;
            }

            if (z == z->parent->left) {
                /* Case II: */
                z = z->parent;
                if (rb_right_rotate(t, z) != RB_OK)
                    return RB_INTERNAL_ERR;
            }

            /* Case III: */
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

int rb_delete(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *z = node, *y, *x;
    rb_color_t y_color;

    if (t == NULL || node == NULL) return RB_NULL_PARAM;

    /* All tree manipulations were implemented based on "Red-Black Trees"
     * chapter from "Introduction to Algorithms". */

    y = z;
    y_color = y->color;
    if (z->left == NULL) {
        x = z->right;
        rb_transplant(t, z, z->right);

    } else if (z->right == NULL) {
        x = z->left;
        rb_transplant(t, z, z->left);

    } else {
        y = rb_minimum(z->right);
        y_color = y->color;
        x = y->right;

        if (y->parent == z) {
            x->parent = y;
        } else {
            rb_transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rb_transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_color == RB_BLACK) {
        rb_delete_fixup(t, x);
    }

    return RB_OK;
}

int rb_delete_fixup(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *x = node, *w;

    /* All tree manipulations were implemented based on "Red-Black Trees"
     * chapter from "Introduction to Algorithms". */

    while (x != t->root && x->color == RB_BLACK) {

        if (x == x->parent->left) {

            w = x->parent->right;

            if (IS_RED(w)) {
                /* Case I: */
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_left_rotate(t, x->parent);
                w = x->parent->right;
            }

            if (IS_BLACK(w->left) && IS_BLACK(w->right)) {
                /* Case II:  */
                w->color = RB_RED;
                x = x->parent;

            } else {

                if (IS_BLACK(w->right)) {
                    /* Case III: */
                    w->left->color = RB_BLACK;
                    w->color = RB_RED;
                    rb_right_rotate(t, w);
                    w = x->parent->right;
                }

                /* Case IV: */
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->right->color = RB_BLACK;
                rb_left_rotate(t, x->parent);
                x = t->root;
            }

        } else if (x == x->parent->right) {

            w = x->parent->left;

            if (IS_RED(w)) {
                /* Case I: */
                w->color = RB_BLACK;
                x->parent->color = RB_RED;
                rb_right_rotate(t, x->parent);
                w = x->parent->left;
            }

            if (IS_BLACK(w->left) && IS_BLACK(w->right)) {
                /* Case II:  */
                w->color = RB_RED;
                x = x->parent;

            } else {

                if (IS_BLACK(w->left)) {
                    /* Case III: */
                    w->right->color = RB_BLACK;
                    w->color = RB_RED;
                    rb_left_rotate(t, w);
                    w = x->parent->left;
                }

                /* Case IV: */
                w->color = x->parent->color;
                x->parent->color = RB_BLACK;
                w->left->color = RB_BLACK;
                rb_right_rotate(t, x->parent);
                x = t->root;
            }

        } else {
            /* Should never happen, but just to make sure... */
            return RB_INTERNAL_ERR;
        }
    }

    return RB_OK;
}

rb_node_t* rb_first(rb_tree_t *t) {
    /* Find the element with the smallest value in the whole tree (it is the
     * first element in depth-first in-order traversal. */
    return rb_minimum(t->root);
}

rb_node_t* rb_next(rb_node_t *node) {

    if (node == NULL) return NULL;

    /* If right subtree is not empty of current node, then that's where we
     * should look for the next element. More precisely, we have to move to the
     * right child and then as far left as possible. */
    if (node->right != NULL) {
        node = node->right;
        while (node->left != NULL) node = node->left;
        return node;
    }

    /* Right subtree is empty. Go up until we find a node that is a left
     * child of it's parent. Parent of such a node is the one we're looking
     * for. */
    while (node->parent != NULL && node->parent->right == node) {
        node = node->parent;
    }
    return node->parent;
}

int rb_print_subtree(rb_node_t *subtree, void (print_element)(void *element)) {
    return _rb_print_subtree(subtree, print_element, "", false);
}

/* Private functions -------------------------------------------------------- */
static int _rb_print_subtree(rb_node_t *subtree,
        void (print_element)(void *element), char *prefix, bool is_tail) {
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

static int rb_left_rotate(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *x = node, *y;

    if (x == NULL || t == NULL) return RB_NULL_PARAM;

    /* All tree manipulations were implemented based on "Red-Black Trees"
     * chapter from "Introduction to Algorithms". */

    /* Selected node has to have right child to rotate left */
    if (x->right == NULL) return RB_CANT_ROTATE;

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
    y = x->right;
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

static int rb_right_rotate(rb_tree_t *t, rb_node_t *node) {
    rb_node_t *x = node, *y;

    if (x == NULL || t == NULL) return RB_NULL_PARAM;

    /* All tree manipulations were implemented based on "Red-Black Trees"
     * chapter from "Introduction to Algorithms". */

    /* Selected node has to have left child to rotate right */
    if (x->left == NULL) return RB_CANT_ROTATE;

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
    y = x->left;
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

static int rb_transplant(rb_tree_t *t, rb_node_t *u, rb_node_t *v) {

    if (t == NULL || u == NULL) return RB_NULL_PARAM;

    if (u->parent == NULL) {
        t->root = NULL;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }

    v->parent = u->parent;

    return RB_OK;
}

static rb_node_t* rb_minimum(rb_node_t *node) {

    if (node == NULL) return NULL;

    while (node->left != NULL) node = node->left;

    return node;
}
