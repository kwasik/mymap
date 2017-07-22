/*
 * mymap.c
 *
 *  Created on: 18.07.2017
 *      Author: krystian
 */

#include "mymap.h"
#include "stddef.h"

/* Exported functions ------------------------------------------------------- */
int mymap_init(map_t *map) {
    if (map == NULL) return MYMAP_ERR;

    /* Initialize red-black tree of mapped regions */
    if (rb_init(&map->rb_tree) != RB_OK) return MYMAP_ERR;

    return MYMAP_OK;
}

int mymap_dump(map_t *map) {
    return MYMAP_ERR;
}

void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags,
        void *o) {

    if (map == NULL) return MYMAP_FAILED;

    if (RB_EMPTY(map->rb_tree)) {

        /* This will be the first region in the address space, so we can map it
         * to the suggested address */
        if (vaddr != NULL) {
            // ...

        } else {
            /* Suggested address was not provided, map to the beginning of the
             * address space */
            // ...
        }
    }

    // ...

    return MYMAP_FAILED;
}

void mymap_munmap(map_t *map, void *vaddr) {
    rb_node_t *node;
    map_region_t *region;

    if (map == NULL) return;

    /* Find region this address belongs to */
    // ...

    /* Remove region from the tree */
    rb_delete(map->rb_tree, node);
    MYMAP_FREE(node);
    MYMAP_FREE(region);
}
