#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "dir/dir_entry.h"
#include "cluster/data_cluster.h"
#include "utils.h"

#define CLUSTER_SIZE 1024

uint16_t fat[4096]; // FAT's table 8 clusters, 4096 inputs of 16 bits (8192 bytes)
dir_entry_t root[32];

FILE *fatPartition;

void loadFileSystem() {

    // Load the fat partition
    fatPartition = fopen("fat.part", "r+");
    fseek( fatPartition, CLUSTER_SIZE, SEEK_SET );
    fread( fat, CLUSTER_SIZE, 8, fatPartition );

    fseek( fatPartition, 5120, SEEK_SET );
    fread( root, CLUSTER_SIZE, 1, fatPartition );

}

/*
   Deal with the user input and decide which file system fuctio to call
*/
void handleUserInput(char userInput[100]) {

    if (strcmp(userInput, "init") == 0) {

        initializateFileSystem();

    } else if (strcmp(userInput, "load") == 0) {

        loadFileSystem();

    } else if (strcmp(userInput, "mkdir") == 0) {
        
        makeDirectory();

    } else if (strcmp(userInput, "write") == 0) {
        
        writeFile();

    } else if (strcmp(userInput, "create") == 0) {
        
        makeFile();

    } else if (strcmp(userInput, "ls") == 0) {
        
        listDirectories();

    } else {
        
        printf("Invalid Command");

    }

}

int main() {

    system("clear");
    printf("File System\n");
    char userInput[100];

    while(strcmp(userInput, "quit") != 0) {

        scanf("%s", userInput);
        handleUserInput(userInput);

    }


    return 0;
}