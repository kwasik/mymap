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
    unsigned int elements[] = {0, 1, 2, 3, 4, 5, 6, 7};
    rb_node_t nodes[8];

    memset(nodes, 0, sizeof(nodes));

    nodes[0].element = (void*)&elements[0];
    nodes[1].element = (void*)&elements[1];
    nodes[2].element = (void*)&elements[2];
    nodes[3].element = (void*)&elements[3];
    nodes[4].element = (void*)&elements[4];
    nodes[5].element = (void*)&elements[5];
    nodes[6].element = (void*)&elements[6];
    nodes[7].element = (void*)&elements[7];

    tree.root = &nodes[0];
    nodes[0].left = &nodes[1];
    nodes[0].right = &nodes[2];
    nodes[1].left = &nodes[3];
    nodes[1].right = &nodes[4];
    nodes[4].left = &nodes[5];
    nodes[4].right = &nodes[6];
    nodes[6].right = &nodes[7];

    rb_print_subtree(tree.root, print_uint);

    return 0;
}
