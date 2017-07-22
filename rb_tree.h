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
    int (*compare)(rb_node_t*, rb_node_t*);
} rb_tree_t;

/* Exported functions ------------------------------------------------------- */
int rb_init(rb_tree_t *t, int (*compare)(rb_node_t*, rb_node_t*));
int rb_insert_fixup(rb_tree_t *t, rb_node_t *node);
int rb_delete(rb_tree_t *t, rb_node_t *node);
int rb_delete_fixup(rb_tree_t *t, rb_node_t *node);

/**
 * Returns first node in depth-first in-order traversal
 * @param t Pointer to the tree
 * @return Pointer to the first node
 */
rb_node_t* rb_first(rb_tree_t *t);

/**
 * Returns next node in depth-first in-order traversal
 * @param node Current node
 * @return Pointer to the next node or NULL if current node is the last
 */
rb_node_t* rb_next(rb_node_t *node);

int rb_print_subtree(rb_node_t *subtree, void (print_element)(void *element));
