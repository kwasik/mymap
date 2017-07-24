/*
 * r_btree.h
 *
 *  Created on: Jul 19, 2017
 *      Author: krystian
 */

#include <stddef.h>

/* Settings ----------------------------------------------------------------- */
#include <stdlib.h>
#define RB_MALLOC(size)         malloc(size)
#define RB_FREE(ptr)            free(ptr)

#include <stdio.h>
#define RB_PRINTF(...)          printf(__VA_ARGS__)

/* Return codes ------------------------------------------------------------- */
#define RB_OK                   (0)
#define RB_ERR                  (-1) /* Unspecified error */
#define RB_ALLOC_ERR            (-2) /* Memory allocation error */
#define RB_NULL_PARAM           (-3) /* Null parameter error */
#define RB_INTERNAL_ERR         (-4) /* Internal error */
#define RB_CANT_ROTATE          (-5) /* Cannot rotate on selected node */

/* Exported macros and definitions ------------------------------------------ */
#define RB_EMPTY(tree)          ((tree)->root == NULL)

#define RB_ELEMENT(node, type)  ((type*)node->element)

#define RB_LINK_LEFT(_parent, _child)                                       \
    _parent->left = _child;                                                 \
    _child->parent = _parent;

#define RB_LINK_RIGHT(_parent, _child)                                      \
    _parent->right = _child;                                                \
    _child->parent = _parent;

/* Exported types ----------------------------------------------------------- */
typedef enum {
    RB_RED,
    RB_BLACK,
} rb_color_t;

typedef struct rb_node_s rb_node_t;

struct rb_node_s {
    void *element;
    rb_node_t *parent;
    rb_node_t *left;
    rb_node_t *right;
    rb_color_t color;
};

typedef struct {
    rb_node_t *root;
} rb_tree_t;

/* Exported functions ------------------------------------------------------- */
/**
 * Initializes red-black tree.
 * @param t Pointer to the tree instance
 * @return Returns zero on success and error code otherwise
 */
int rb_init(rb_tree_t *t);

/**
 * Initializes node of red-black tree.
 * @param node Pointer to the node
 * @param element Pointer to the element to store in the node
 */
void rb_node_init(rb_node_t *node, void *element);

/**
 * Modifies tree so that it becomes a red-black tree again after inserting new
 * node. Node should be inserted according to rules imposed on binary search
 * trees.
 * @param t Pointer to the tree instance
 * @param node Pointer to the inserted node
 * @return Returns zero on success and error code otherwise
 */
int rb_insert_fixup(rb_tree_t *t, rb_node_t *node);

/**
 * Removes node and modifies the tree so it still is a valid red-black
 * tree.
 * @param t Pointer to the tree instance
 * @param node Pointer to the node to remove
 * @return Returns zero on success and error code otherwise
 */
int rb_delete(rb_tree_t *t, rb_node_t *node);

/**
 * Returns first node in depth-first in-order traversal.
 * @param t Pointer to the tree
 * @return Pointer to the first node or NULL if the tree is empty
 */
rb_node_t* rb_first(rb_tree_t *t);

/**
 * Returns next node in depth-first in-order traversal.
 * @param node Current node
 * @return Pointer to the next node or NULL if current node is the last
 */
rb_node_t* rb_next(rb_node_t *node);

/**
 * Returns previous node in depth-first in-order traversal.
 * @param node Current node
 * @return Pointer to the previous node or NULL if current node is the first
 */
rb_node_t* rb_previous(rb_node_t *node);

/**
 * Searches for a given key in the tree
 * @param t Pointer to the tree
 * @param element Pointer to the key to search for
 * @param compare Compare function accepting two parameters, key and element
 * stored in a tree node. Defines the order of elements by returning -1, 0 or 1
 * if the key is lesser, equal or greater than the element stored in the node,
 * respectively
 * @param result Pointer to place where the result of last comparison will be
 * stored
 * @return Pointer to the node with element equal to the key. If there is no
 * such an element in the tree, returns pointer to the node it should be
 * attached to. May return NULL if an error occurs or if the tree is empty.
 */
rb_node_t* rb_search(rb_tree_t *t, void *key, int (*compare)(void*, void*),
        int *result);

/**
 * Prints subtree in human-readable form
 * @param subtree Pointer to the root of the subtree
 * @param print_element Pointer to the function that will be used to print
 * elements stored in the tree
 * @return Returns zero on success and error code otherwise
 */
int rb_print_subtree(rb_node_t *subtree, void (print_element)(void *element));
