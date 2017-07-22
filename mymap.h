/*
 * mymap.h
 *
 *  Created on: 18.07.2017
 *      Author: krystian
 */

#ifndef MYMAP_H_
#define MYMAP_H_

#include <sys/mman.h>

/* Return codes ------------------------------------------------------------- */
#define MYMAP_OK                ((int)0)
#define MYMAP_ERR               ((int)-1)	/* Unspecified error */

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
    map_region_t *left; /* Left child in red-black tree */
    map_region_t *right; /* Right child in red-black tree */
};

typedef struct {
    map_region_t *root; /* Root of red-black tree of mapped areas */
    void *mstart; /* Lowest address in the virtual address space */
    void *mend; /* Lowest address that is greater than all addresses in virtual
                 * address space */
} map_t;

/* Exported functions ------------------------------------------------------- */

/**
 * Initializes map.
 * @param map Pointer to the map instance.
 * @return Returns zero if function succeeds. Otherwise returns error code.
 */
int mymap_init(map_t *map);

/**
 * Dumps structure of the map in human-readable format to stdout.
 * @param map Pointer to the map instance.
 * @return Returns zero if function succeeds. Otherwise returns error code.
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
 * @return Address the region was mapped to.
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
