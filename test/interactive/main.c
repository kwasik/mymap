/*
 * main.c
 *
 *  Created on: 24.07.2017
 *      Author: krystian
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mymap.h"

#define ARG_LEN                 (128)
#define ARG_NUM                 (16)

typedef struct {
    char *name;
    bool (*func)(int, char**);
} command_t;

#define NUM_OF_COMMANDS         (sizeof(commands)/sizeof(command_t))

bool run_command(int argc, char **argv);

bool help(int argc, char **argv);
bool mmap(int argc, char **argv);
bool unmap(int argc, char **argv);
bool dump(int argc, char **argv);

command_t commands[] = {
        {"help", help},
        {"mmap", mmap},
        {"unmap", unmap},
        {"dump", dump},
};

map_t map;

int main(int argc, char **argv) {
    char *args[ARG_NUM];

    /* Initialize map */
    printf("Initializing map... ");
    if (mymap_init(&map) == MYMAP_OK) {
        printf("OK.\n");
    } else {
        printf("FAILED.\n");
        exit(EXIT_FAILURE);
    }

    /* Add some elements to map */
    mymap_mmap(&map, (void*)0x100, 10, 0, (void*)0);
    mymap_mmap(&map, (void*)0x200, 10, 0, (void*)0);
    mymap_mmap(&map, (void*)0xf0, 10, 0, (void*)0);

    while (true) {

        /* Display command prompt */
        printf("> ");

        /* Get next command with arguments */
        char *line = NULL;
        size_t line_length = 0;
        if (getline(&line, &line_length, stdin) == -1) exit(EXIT_FAILURE);

        /* Parse command */
        unsigned num_of_args = 0;
        char *token = strtok(line, " ");
        while (token != NULL) {

            /* Get rid of newline character */
            size_t token_length = strlen(token);
            if (token[token_length - 1] == '\n')
                token[token_length - 1] = 0;

            /* Store argument */
            args[num_of_args] = token;
            num_of_args++;

            /* Get next token */
            token = strtok(NULL, " ");
        }

        run_command(num_of_args, args);

        free(line);
    }

    return 0;
}

bool run_command(int argc, char **argv) {
    unsigned i;

    if (argc <= 0) return true;

    for (i = 0; i < NUM_OF_COMMANDS; i++) {
        if (strcmp(commands[i].name, argv[0]) == 0) {
            return commands[i].func(argc, argv);
        }
    }

    printf("Unrecognized command. For help, type \"help\".\n");
    return false;
}

bool help(int argc, char **argv) {
    printf("help called.\n");
    return true;
}

bool mmap(int argc, char **argv) {
    printf("mmap called.\n");
    return true;
}

bool unmap(int argc, char **argv) {
    printf("unmap called.\n");
    return true;
}

bool dump(int argc, char **argv) {
    mymap_dump(&map);
    return true;
}
