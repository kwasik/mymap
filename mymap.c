/*
 * mymap.c
 *
 *  Created on: 18.07.2017
 *      Author: krystian
 */

#include "mymap.h"
#include "stddef.h"

/* Private functions -------------------------------------------------------- */
mymap_get_unmapped_area(map_t *map, void *vaddr, unsigned int size);

/* Exported functions ------------------------------------------------------- */
int mymap_init(map_t *map) {
    return MYMAP_ERR;
}

int mymap_dump(map_t *map) {
    return MYMAP_ERR;
}

/* TODO: How to distinguish between mapping to address 0x0 and returning error
 * (NULL pointer)? */
void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags,
        void *o) {

    if (map == NULL) return NULL;

    /* Make sure suggested address is inside the address space */
    if (vaddr < map->mend) return NULL;

    if (RB_EMPTY(map->root)) {
        /* This will be the first region in the address space, so we can map it
         * precisely to the suggested address */
        if (vaddr != NULL) {
            // ...
        } else {
            /* Suggested address was not provided, map to the beginning of the
             * address space */
            // ...
        }
    }

    // ...
}

void mymap_munmap(map_t *map, void *vaddr) {

}
