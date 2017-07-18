/*
 * mymap.h
 *
 *  Created on: 18.07.2017
 *      Author: krystian
 */

#ifndef MYMAP_H_
#define MYMAP_H_

#include <sys/mman.h>

/* Exported types ----------------------------------------------------------- */
typedef struct {
	/* TODO: Implement map_t */
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
