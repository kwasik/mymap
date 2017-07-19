/*
 * main.c
 *
 *  Created on: Jul 19, 2017
 *      Author: krystian
 */

#include <stdio.h>
#include <string.h>
#include "rbtree.h"

void print_uint(void *num) {
    printf("%u", *((unsigned int*)num));
}

int main(int argc, char **argv) {
    rb_tree_t tree;

    rb_init(&tree, NULL);

    /* Add a few nodes */
    unsigned int elements[] = {0, 1, 2, 3, 4};
    rb_node_t nodes[5];

    memset(nodes, 0, sizeof(nodes));

    nodes[0].element = (void*)&elements[0];
    nodes[1].element = (void*)&elements[1];
    nodes[2].element = (void*)&elements[2];
    nodes[3].element = (void*)&elements[3];
    nodes[4].element = (void*)&elements[4];

    tree.root = &nodes[0];
    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];
    nodes[1].left = &nodes[3];
    nodes[1].right = &nodes[4];

    rb_print_subtree(tree.root, print_uint);

    return 0;
}
