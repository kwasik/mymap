/*
 * mymap.h
 *
 *  Created on: 18.07.2017
 *      Author: krystian
 */

#ifndef MYMAP_H_
#define MYMAP_H_

#include "rb_tree.h"

/* Settings ----------------------------------------------------------------- */
#include <stdlib.h>
#define MYMAP_MALLOC(size)      malloc(size)
#define MYMAP_FREE(ptr)         free(ptr)

#include <stdio.h>
#define MYMAP_PRINTF(...)       printf(__VA_ARGS__)

/* Base of the virtual address space (smallest available address) */
#define MYMAP_VA_BASE           ((void*)0x00000010)

/* End (last address) of the virtual address space */
#define MYMAP_VA_END            ((void*)0x00001000)

/* Return codes ------------------------------------------------------------- */
#define MYMAP_OK                (0)
#define MYMAP_ERR               (-1)    /* Unspecified error */

#define MYMAP_FAILED            ((void*)MYMAP_ERR)

/* Memory region flags ------------------------------------------------------ */
#define MYMAP_READ              (1 << 0)	/* Marks readable region */
#define MYMAP_WRITE             (1 << 1)	/* Marks writable region */
#define MYMAP_EXEC              (1 << 2)	/* Marks executable region */

/* Exported types ----------------------------------------------------------- */
typedef struct map_region_s map_region_t;

struct map_region_s {
    void *paddr; /* Physical address of the first byte inside the region */
    void *vaddr; /* Virtual address of the first byte inside the region */
    void *vend; /* Virtual address of the first byte after the region */
    unsigned int flags; /* Memory region flags */
    rb_node_t *rb_node; /* Node of a red-black tree this region is stored in */
    unsigned long gap; /* Gap before this region */
    unsigned long max_gap; /* Largest unmapped area in the subtree */
};

typedef struct {
    rb_tree_t rb_tree; /* Red-black tree of mapped areas */
    unsigned long last_gap; /* Size of the area between the last region and the
                             * end of the address space */
} map_t;

/* Exported functions ------------------------------------------------------- */

/**
 * Initializes map.
 * @param map Pointer to the map instance.
 * @return Returns zero if operation succeeds. Otherwise returns error code.
 */
int mymap_init(map_t *map);

/**
 * Dumps structure of the map in human-readable format to stdout.
 * @param map Pointer to the map instance.
 * @return Returns zero if operation succeeds. Otherwise returns error code.
 */
int mymap_dump(map_t *map);

/**
 * Maps region defined by arguments to process address space to address greater
 * or equal than suggested address.
 * Returns address the region was mapped to.
 * @param map Pointer to the map instance.
 * @param vaddr Suggested address to map to.
 * @param size Size of the mapped region.
 * @param flags Mapping attributes.
 * @param o Address of the beginning of the mapped region.
 * @return On success, returns address the region was mapped to. On failure,
 * MYMAP_FAILED is returned.
 */
void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags,
        void *o);

/**
 * Unmaps region containing address passed as a parameter.
 * @param map Pointer to the map instance.
 * @param vaddr Any address from the region to unmap.
 */
void mymap_munmap(map_t *map, void *vaddr);

/**
 * Allocates and initializes region and tree node structures
 * @param paddr Physical address of the region
 * @param flags Memory region flags
 * @return Pointer to the region structure if operation succeeds. Otherwise
 * returns NULL.
 */
map_region_t* mymap_create_region(void *paddr, unsigned int flags);

/**
 * Searches the tree of regions to find a right place for a new region (gap big
 * enough and virtual address greater or equal to suggested in parameter)
 * @param map Pointer to the map instance
 * @param vaddr Suggested virtual address
 * @param size Size of the new region
 */
void* mymap_get_unmapped_area(map_t *map, void *vaddr,
        unsigned int size);

#endif /* MYMAP_H_ */
