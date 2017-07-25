/*
 * mymap.c
 *
 *  Created on: 18.07.2017
 *      Author: krystian
 */

#include "mymap.h"
#include <stddef.h>
#include <stdbool.h>

/* Private macros ----------------------------------------------------------- */
#define RB_MAX_GAP(node)        RB_ELEMENT(node, map_region_t)->max_gap
#define RB_GAP(node)            RB_ELEMENT(node, map_region_t)->gap
#define RB_VADDR(node)          RB_ELEMENT(node, map_region_t)->vaddr

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
static inline void* mymap_check_last_gap(map_t *map, void *vaddr,
        unsigned long size);
static void* mymap_get_unmapped_area(map_t *map, void *vaddr,
        unsigned int size);
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
        region->vaddr = mymap_get_unmapped_area(map, vaddr, size);

        /* TODO: Insert new region */
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

static inline void* mymap_check_last_gap(map_t *map, void *vaddr,
        unsigned long size) {
    void *gap_start;

    if (map == NULL) return MYMAP_FAILED;

    gap_start = MYMAP_VA_END - map->last_gap + 1;

    if (gap_start > vaddr && map->last_gap > size) {
        return gap_start;
    } else if (gap_start <= vaddr && (MYMAP_VA_END - vaddr + 1) > size) {
        return vaddr;
    } else {
        return MYMAP_FAILED;
    }
}

static void* mymap_get_unmapped_area(map_t *map, void *vaddr,
        unsigned int size) {
    rb_node_t *curr;

    if (map == NULL) return MYMAP_FAILED;

    if (RB_EMPTY(&map->rb_tree) || RB_MAX_GAP(map->rb_tree.root) < size)
        /* If tree is empty or maximum gap size at the root is smaller than
         * requested size, then the last gap is our only chance */
        return mymap_check_last_gap(map, vaddr, size);


    /* In the first phase we analyze parts of the tree where we have to watch
     * out both for maximum gap size in subtree and suggested virtual address.
     * We'll make sure that suggested address is located before current region
     * before moving to the second phase. */
    curr = map->rb_tree.root;
    while (true) {

        map_region_t *region = RB_ELEMENT(curr, map_region_t);
        int result = mymap_belongs_to_region(vaddr, region);

        if (result < 0) { /* vaddr is before the current region */

            if (curr->left == NULL) {

                /* There is no left subtree. Check if we can insert new region
                 * before the current one */
                map_region_t *tmp = RB_ELEMENT(curr->left, map_region_t);
                if (tmp->gap >= size && (tmp->vaddr - vaddr) >= size) {
                    return vaddr;
                } else {
                    /*  Won't fit here. Go to the second phase. */
                    break;
                }

            /* Check if any gap in the left subtree is big enough */
            } else if (RB_MAX_GAP(curr->left) >= size) {
                curr = curr->left;

            } else {
                /* Won't fit into this subtree. Go to the seconds phase. */
                break;
            }

        } else if (result == 0) {

            /* vaddr is inside the current region. Move to the next element and
             * go to the second phase. If no such an element exists, check if we
             * can place new region after current one (and the last at the same
             * time). */
            rb_node_t *next = rb_next(curr);
            if (next == NULL)
                return mymap_check_last_gap(map, vaddr, size);
            curr = next;
            break;

        } else { /* vaddr is after the current region */

            if (curr->right == NULL) {

                /* There is no right subtree. Move to the next element and go to
                 * the second phase. If no such an element exists, check the
                 * last gap. */
                rb_node_t *next = rb_next(curr);
                if (next == NULL)
                    return mymap_check_last_gap(map, vaddr, size);
                curr = next;
                break;

            /* Check if right subtree looks promising */
            } else if (RB_MAX_GAP(curr->right) < size) {

                /* Move to the next element skipping whole right subtree and go
                 * to the second phase. If such element doesn't exist, check the
                 * last gap. */
                rb_node_t *tmp = rb_subtree_next(curr);
                if (tmp == NULL)
                    return mymap_check_last_gap(map, vaddr, size);
                curr = tmp;
                break;
            }

            /* Look deeper into the right subtree */
            curr = curr->right;
        }
    }

    /* In second phase we no longer have to worry about the suggested virtual
     * address, since in the previous part we made sure that is located before
     * the current region. */
    while (true) {

        /* Check if gap before current element is big enough */
        void *gap_start = (void*)(RB_VADDR(curr) - RB_GAP(curr));
        if (gap_start > vaddr && RB_GAP(curr) > size) {
            return gap_start;
        } else if (gap_start <= vaddr && (RB_VADDR(curr) - vaddr) > size) {
            return vaddr;
        }

        /* Check if there is a gap big enough in the right subtree */
        if (curr->right && RB_MAX_GAP(curr->right) > size) {

            curr = curr->right;
            while (true) {

                /* We want to get as close to suggested address as possible and
                 * therefore we should start with left subtree */
                if (curr->left && RB_MAX_GAP(curr->left) > size) {
                    curr = curr->left;

                /* Check current element */
                } else if (RB_GAP(curr) > size) {
                    return RB_VADDR(curr) - RB_GAP(curr);

                /* Check right subtree as a last resort */
                } else if (curr->right && RB_MAX_GAP(curr->left) > size) {
                    curr = curr->right;

                } else {
                    /* Should never happen unless tree is broken */
                    return MYMAP_FAILED;
                }
            }
        }

        /* Move to the next element skipping whole right subtree and go
         * to the second phase. If such element doesn't exist, check the
         * last gap. */
        rb_node_t *tmp = rb_subtree_next(curr);
        if (tmp == NULL)
            return mymap_check_last_gap(map, vaddr, size);
        curr = tmp;
    }

    return NULL;
}

static void mymap_print_region(void *element) {
    map_region_t *r = (map_region_t*)element;

    MYMAP_PRINTF("(paddr: %p, vaddr: %p, vend: %p, flags: %u)", r->paddr,
            r->vaddr, r->vend, r->flags);
}
