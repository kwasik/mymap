/*
 * main.c
 *
 *  Created on: 24.07.2017
 *      Author: krystian
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mymap.h"

/* Number of regions in the virtual memory */
#define NUM_OF_REGIONS              (16)

/* Number of tests. Each consisting of single try to find area for new, random
 * region. */
#define NUM_OF_TESTS                (32)

typedef struct {
    void *vaddr;
    void *vend;
    unsigned long gap;
} _region_t;

/* Region descriptors */
_region_t _regions[NUM_OF_REGIONS];

/* Size of the last gap */
unsigned long last_gap;

/* Virtual memory map instance */
map_t map;

/* Private functions -------------------------------------------------------- */
static void generate_layout(void);
static void print_layout(void);
static void* _get_unmapped_area(_region_t r[], void *vaddr, unsigned int size);
static void* get_random_vaddr(void);
static unsigned int get_random_size(void *vaddr);
static void build_map(map_t *m);
static rb_node_t* _build_map(_region_t *r, size_t size);

/* Main --------------------------------------------------------------------- */
int main(int argc, char **argv) {
    unsigned i = 0;

    srand(time(NULL));

    /* Generate and print random, but valid virtual memory layout */
    generate_layout();
    print_layout();

    /* Build and dump tree */
    printf("TREE STRUCTURE:\n\n");
    build_map(&map);
    mymap_dump(&map);

    /* Wait for any key */
    printf("\nPress any key to run tests...\n");
    getchar();


    /* Display header */
    printf("\n%4s %10s %10s %20s %20s\n", "nr", "vaddr", "size", "array",
            "tree");

    while (1) {

        /* Get random region */
        void *array_addr, *tree_addr, *vaddr = get_random_vaddr();
        unsigned size = get_random_size(vaddr);

        /* Map region using both methods */
        array_addr = _get_unmapped_area(_regions, vaddr, size);
        tree_addr = mymap_get_unmapped_area(&map, vaddr, size);

        /* Display results */
        printf("%4u %10p %10u %20p %20p\n", i, vaddr, size, array_addr,
                tree_addr);

        /* If results differ, dump tree and display mmemory layout */
        if (array_addr != tree_addr) {
            print_layout();
            mymap_dump(&map);

            /* Wait for any key */
            getchar();
        }

        if (i >= NUM_OF_TESTS) break;
        i++;
    }

    return 0;
}

/* Private functions -------------------------------------------------------- */
static void generate_layout(void) {
    unsigned i;
    unsigned long va_size = (unsigned long)(MYMAP_VA_END - MYMAP_VA_BASE), size;
    void *low_limit, *high_limit;

    low_limit = MYMAP_VA_BASE;
    high_limit = low_limit + va_size/NUM_OF_REGIONS;
    for (i = 0; i < NUM_OF_REGIONS; i++) {

        /* Rand start address */
        size = high_limit - low_limit;
        _regions[i].vaddr = low_limit + (long)rand()*size/RAND_MAX;
        if (_regions[i].vaddr < low_limit) _regions[i].vaddr = low_limit;

        /* Rand end address */
        size = high_limit - _regions[i].vaddr;
        _regions[i].vend = _regions[i].vaddr + (long)rand()*size/RAND_MAX;
        if (_regions[i].vend > high_limit) _regions[i].vend = high_limit;

        /* Calculate gap size */
        if (i == 0) {
            _regions[i].gap = _regions[i].vaddr - MYMAP_VA_BASE;
        } else {
            _regions[i].gap = _regions[i].vaddr - _regions[i - 1].vend;
        }

        /* Designate limits for the next region */
        low_limit = high_limit + 1;
        high_limit = low_limit + va_size/NUM_OF_REGIONS;
        if (high_limit > MYMAP_VA_END) high_limit = MYMAP_VA_END;
    }

    /* Calculate last gap size */
    last_gap = MYMAP_VA_END - _regions[NUM_OF_REGIONS - 1].vend + 1;
}

static void print_layout(void) {
    unsigned i;

    printf("\nMEMORY LAYOUT:\n\n");
    printf("Nr %10s %10s %10s\n", "vstart", "vend", "gap");
    for (i = 0; i < NUM_OF_REGIONS; i++) {
        printf("%2u %10p %10p %10lu\n", i, _regions[i].vaddr, _regions[i].vend,
                (unsigned long)_regions[i].gap);
    }

    printf("\nLast gap = %10lu\n\n", last_gap);
}

static void* _get_unmapped_area(_region_t r[], void *vaddr, unsigned int size) {
    unsigned i;
    void *start, *end;

    if (r == NULL) return MYMAP_FAILED;

    for (i = 0; i < NUM_OF_REGIONS; i++) {

        if (vaddr < r[i].vaddr) {

            /* Get gap start and end */
            if (i == 0) {
                start = MYMAP_VA_BASE;
            } else {
                start = r[i - 1].vend;
            }
            end = r[i].vaddr;

            /* Check if we can fit new region here */
            if (start < vaddr && (end - vaddr) >= size) {
                return vaddr;
            } else if (start >= vaddr && (end - start) >= size) {
                return start;
            }
        }
    }

    /* Check the last gap */
    start = r[NUM_OF_REGIONS - 1].vend;
    end = MYMAP_VA_END + 1;
    if (start < vaddr && (end - vaddr) > size) {
        return vaddr;
    } else if (start >= vaddr && (end - start) > size) {
        return start;
    }

    return MYMAP_FAILED;
}

static void* get_random_vaddr(void) {
    return (void*)(rand()*(MYMAP_VA_END + 1 - MYMAP_VA_BASE)/RAND_MAX);
}

static unsigned int get_random_size(void *vaddr) {
    return rand()*(MYMAP_VA_END + 1 - vaddr)/RAND_MAX/2;
}

static void build_map(map_t *m) {
    if (m == NULL) return;

    m->rb_tree.root = _build_map(_regions, NUM_OF_REGIONS);
    m->last_gap = last_gap;
}

static rb_node_t* _build_map(_region_t *r, size_t size) {
    unsigned index = size/2;
    map_region_t *region = mymap_create_region(NULL, 0);
    rb_node_t* node = region->rb_node, *child;

    /* Fill-in region structure */
    region->vaddr = r[index].vaddr;
    region->vend = r[index].vend;
    region->gap = r[index].gap;
    region->max_gap = r[index].gap;

    /* Build left subtree */
    if (index > 0) {
        child = _build_map(r, index);
        RB_LINK_LEFT(node, child);
        if (region->max_gap < RB_ELEMENT(child, map_region_t)->max_gap) {
            region->max_gap = RB_ELEMENT(child, map_region_t)->max_gap;
        }
    }

    /* Build right subtree */
    if ((size - index - 1) > 0) {
        child = _build_map(&r[index + 1], size - index - 1);
        RB_LINK_RIGHT(node, child);
        if (region->max_gap < RB_ELEMENT(child, map_region_t)->max_gap) {
            region->max_gap = RB_ELEMENT(child, map_region_t)->max_gap;
        }
    }

    return node;
}
