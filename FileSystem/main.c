#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dir/dir_entry.h"
#include "cluster/data_cluster.h"
#include "utils.h"

uint16_t fat[4096]; // FAT's table 8 clusters, 4096 inputs of 16 bits (8192 bytes)

int main() {
    char* init = "init";
    bool quit = false;

    while (!quit) {
        char[50] command;

        // TODO: read input command

        if (starts_with(init, &command)) {
            // TODO: implement init function
        }

        // TODO: implement other commands
    }

    return 0;
}
