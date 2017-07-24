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

/* Base of the virtual address space (smallest available address) */
#define MYMAP_VA_BASE           ((void*)0x00000010)

/* End (last address) of the virtual address space */
#define MYMAP_VA_END            ((void*)0xfffffff0)

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
};

typedef struct {
    rb_tree_t rb_tree; /* Red-black tree of mapped areas */
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

#endif /* MYMAP_H_ */
