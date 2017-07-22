/*
 * main.c
 *
 *  Created on: Jul 19, 2017
 *      Author: krystian
 */

#include <stdio.h>
#include <string.h>
#include "rb_tree.h"

/* Return codes */
#define TEST_PASSED         (0)
#define TEST_FAILED         (1)

/* Test functions */
int test1(void);
int test2(void);

/* Test descriptor */
typedef struct {
    int (*run)(void);
    const char *description;
} test_t;

/* Array of test descriptors */
test_t tests[] = {
        {test1, "Basic test"},
        {test2, "Another basic test"},
};

/* Number of all tests */
#define NUM_OF_TESTS        (sizeof(tests)/sizeof(test_t))

int main(int argc, char **argv) {
    unsigned i, failed = 0, passed;

    for (i = 0; i < NUM_OF_TESTS; i++) {
        printf("%s... ", tests[i].description);
        if (tests[i].run() == TEST_PASSED) {
            printf("PASSED.\n");
        } else {
            printf("FAILED.\n");
            failed++;
        }
    }

    passed = NUM_OF_TESTS - failed;

    printf("\nFailed: %u/%lu. Passed: %u/%lu.\n", failed, NUM_OF_TESTS,
            passed, NUM_OF_TESTS);

    return 0;
}

int test1(void) {
    return TEST_PASSED;
}

int test2(void) {
    return TEST_FAILED;
}
