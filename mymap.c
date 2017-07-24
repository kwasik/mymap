/*
 * mymap.c
 *
 *  Created on: 18.07.2017
 *      Author: krystian
 */

#include "mymap.h"
#include <stddef.h>
#include <stdbool.h>

/* Private functions -------------------------------------------------------- */
/**
 * Checks if virtual address belongs to region
 * @param vaddr Virtual address to check
 * @param region Pointer to region structure
 * @return Returns -1, 0 or 1 if virtual address is located before, inside or
 * after region, respectively
 */
static int mymap_belongs_to_region(void *vaddr, void *region);

/* TODO: Comment */
static map_region_t* mymap_create_region(void *paddr, unsigned int flags);
static void mymap_destroy_region(map_region_t *region);
static void* mymap_get_unmapped_area(map_t *map, void *vaddr, unsigned int size,
        map_region_t **after, map_region_t **before);
static unsigned long mymap_get_gap_size(map_region_t *region,
        map_region_t* next, void *vaddr, void **_start);
static bool mymap_insert_region(map_region_t *new, map_region_t *after,
        map_region_t *before);
static void mymap_print_region(void *element);

/* Exported functions ------------------------------------------------------- */
int mymap_init(map_t *map) {
    if (map == NULL) return MYMAP_ERR;

    /* Initialize red-black tree of mapped regions */
    if (rb_init(&map->rb_tree) != RB_OK) return MYMAP_ERR;

    return MYMAP_OK;
}

int mymap_dump(map_t *map) {

    if (map == NULL) return MYMAP_ERR;

    if (RB_EMPTY(&map->rb_tree)) {
        MYMAP_PRINTF("The map is empty.\n");
    } else {
        rb_print_subtree(map->rb_tree.root, mymap_print_region);
    }

    return MYMAP_ERR;
}

void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags,
        void *o) {
    map_region_t *region;

    if (map == NULL) return MYMAP_FAILED;

    region = mymap_create_region(o, flags);
    if (region == NULL) return MYMAP_FAILED;

    if (RB_EMPTY(&map->rb_tree)) {

        /* This will be the first region in the address space, so we can map it
         * to the suggested address if provided and in it is greater than base
         * address of the virtual address space */
        if (vaddr != NULL && vaddr >= MYMAP_VA_BASE) {
            region->vaddr = vaddr;

        } else {
            /* Suggested address was not provided, map to the beginning of the
             * address space */
            region->vaddr = MYMAP_VA_BASE;
        }

        /* Check if we the area is big enough additionally checking for
         * overflow*/
        unsigned long area_size = MYMAP_VA_END - region->vaddr + 1;
        bool overflowed = MYMAP_VA_END > region->vaddr && area_size == 0;
        if (area_size < size && !overflowed) {
            mymap_destroy_region(region); /* Clean up */
            return MYMAP_FAILED;
        }

        map->rb_tree.root = region->rb_node;

    } else {

        /* Get region located before unmapped area fulfilling all
         * requirements*/
        map_region_t *prev, *next;
        region->vaddr = mymap_get_unmapped_area(map, vaddr, size, &next, &prev);

        /* Try to insert new region */
        if (region->vaddr == MYMAP_FAILED ||
                !mymap_insert_region(region, prev, next)) {
            mymap_destroy_region(region); /* Clean up */
            return MYMAP_FAILED;
        }
    }

    /* Calculate the end of the region */
    region->vend = region->vaddr + size;

    /* Fix up the tree of the regions to make sure it is a valid red-black
     * tree */
    if (rb_insert_fixup(&map->rb_tree, region->rb_node) != RB_OK) {
        mymap_destroy_region(region); /* Clean up */
        return MYMAP_FAILED;
    }

    /* Return virtual address the region was mapped to */
    return region->vaddr;
}

void mymap_munmap(map_t *map, void *vaddr) {
    rb_node_t *node;
    int result;

    if (map == NULL) return;

    /* Find region this address belongs to */
    node = rb_search(&map->rb_tree, vaddr, mymap_belongs_to_region, &result);
    if (node == NULL) {

        /* Some error occurred, tree is empty or this address does not belong
         * to any region */
        return;
    }

    /* Remove region from the tree and destroy regions */
    rb_delete(&map->rb_tree, node);
    mymap_destroy_region(RB_ELEMENT(node, map_region_t));
}

/* Private functions -------------------------------------------------------- */
static int mymap_belongs_to_region(void *vaddr, void *region) {
    map_region_t *r = (map_region_t*)region;

    if (vaddr < r->vaddr) {
        return -1;
    } else if (vaddr >= r->vend) {
        return 1;
    } else {
        return 0;
    }
}

static map_region_t* mymap_create_region(void *paddr, unsigned int flags) {
    map_region_t *region;
    rb_node_t *node;

    /* Create and initialize region structure */
    region = MYMAP_MALLOC(sizeof(map_region_t));
    if (region == NULL) return NULL;
    region->paddr = paddr;
    region->flags = flags;

    /* Create and initialize new red-black tree node to store the region in */
    node = MYMAP_MALLOC(sizeof(rb_node_t));
    if (node == NULL) {

        /* Clean up */
        MYMAP_FREE(region);

        return NULL;
    }
    rb_node_init(node, (void*)region);

    /* Create link from region to the corresponding node of red-black tree */
    region->rb_node = node;

    return region;
}

static void mymap_destroy_region(map_region_t *region) {
    MYMAP_FREE(region->rb_node);
    MYMAP_FREE(region);
}

static void* mymap_get_unmapped_area(map_t *map, void *vaddr, unsigned int size,
        map_region_t **after, map_region_t **before) {
    map_region_t *curr, *next;
    void *start;

    if (RB_EMPTY(&map->rb_tree)) return NULL;

    /* If suggested address was provided, find first unused area starting after
     * or containing this address */
    if (vaddr != NULL) {

        /* Search for provided virtual address in the tree of mapped regions */
        int result;
        rb_node_t *n;
        n = rb_search(&map->rb_tree, vaddr, mymap_belongs_to_region, &result);

        if (result < 0) {
            /* Virtual address is located before returned region. There's
             * a chance that we'll be able to fit new region before it.
             * Move back to the previous region. */
            next = RB_ELEMENT(n, map_region_t);
            curr = RB_ELEMENT(rb_previous(n), map_region_t);

        } else {
            curr = RB_ELEMENT(n, map_region_t);
            next = RB_ELEMENT(rb_next(n), map_region_t);
        }

    } else {
        /* Start with area before first region */
        curr = NULL;
        next = RB_ELEMENT(rb_first(&map->rb_tree), map_region_t);
    }

    /* Move forward until we find an area big enough */
    while (true) {

        if (curr == NULL && next == NULL) {
            /* We have reached the end of the address space and haven't found
             * appropriate region */
            return MYMAP_FAILED;
        }

        /* Check if the gap between the regions is big enough */
        if (mymap_get_gap_size(curr, next, vaddr, &start) >= size)
            break;

        /* Move to the next region */
        curr = next;
        next = RB_ELEMENT(rb_next(curr->rb_node), map_region_t);
    }

    /* Return pointer to the regions before and after gap if requested */
    if (after != NULL) *after = curr;
    if (before != NULL) *before = next;

    /* Return address of the beginning of the area */
    return start;
}

static unsigned long mymap_get_gap_size(map_region_t *region,
        map_region_t* next, void *vaddr, void **_start) {
    void *start, *end;

    /* Check if gap starts at the beginning of the address space or at the end
     * of a region */
    if (region != NULL) {
        start = region->vend;
    } else {
        start = MYMAP_VA_BASE;
    }

    /* Check if gap ends at the beginning of the next region or at the end of
     * the address space */
    if (next != NULL) {
        end = next->vaddr - 1;
    } else {
        end = MYMAP_VA_END;
    }

    /* Take into account the fact that the suggested address may be located
     * inside the gap */
    if (vaddr != NULL && vaddr > start) start = vaddr;

    /* Return address of the beginning of the gap if requested */
    if (_start != NULL) *_start = start;

    return (unsigned long)(end - start) + 1;
}

static bool mymap_insert_region(map_region_t *new, map_region_t *after,
        map_region_t *before) {

    if (after == NULL && before == NULL) return false;

    if (after && after->rb_node && after->rb_node->right == NULL) {
        RB_LINK_RIGHT(after->rb_node, new->rb_node);

    } else if (before && before->rb_node && before->rb_node->left == NULL) {
        RB_LINK_LEFT(before->rb_node, new->rb_node);

    } else {
        /* Can happen only if the area where we're trying to insert new region
         * is not empty (which, of course, should never happen) */
        return false;
    }

    return true;
}

static void mymap_print_region(void *element) {
    map_region_t *r = (map_region_t*)element;

    MYMAP_PRINTF("(paddr: %p, vaddr: %p, vend: %p, flags: %u)", r->paddr,
            r->vaddr, r->vend, r->flags);
}
