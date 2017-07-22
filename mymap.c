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
    return MYMAP_ERR;
}

int mymap_dump(map_t *map) {
    return MYMAP_ERR;
}

void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags,
        void *o) {
    return NULL;
}

void mymap_munmap(map_t *map, void *vaddr) {

}
