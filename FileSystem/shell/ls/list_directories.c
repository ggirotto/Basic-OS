#include <stdio.h>
#include <string.h>
#include "list_directories.h"
#include "../../cluster/data_cluster.h"


/*
    This function list all files and folders inside the given path.
    Ex: ls /folder1/folder2
    List all files and folders inside folder2
    For root, type 'ls /'.
*/

void listDirectories(FILE* fatPartition, dir_entry_t* root, uint16_t* address) {
    // Back to root
    fseek(fatPartition, 9*CLUSTER_SIZE, SEEK_SET);
    fread(root, CLUSTER_SIZE, 1, fatPartition);

    char directoryPath[100];
    scanf("%s", directoryPath);

    char *delimiter = "/";
    char *curr = strtok(directoryPath, delimiter);

    while (curr != NULL) {
        // Walk through paths

        bool foundDirectory = false;

        for (int i = 0; i < 32; ++i) {
            dir_entry_t file = root[i];

            if (file.attributes == 0 && strcmp((char *)file.filename, curr) == 0) {
                fseek(fatPartition, file.first_block, SEEK_SET);
                fread(root, CLUSTER_SIZE, 1, fatPartition);
                foundDirectory = true;
                break;
            }

        }

        if (!foundDirectory) {
            printf("DIRECTORY NOT FOUND \n");
            return;
        }

        curr = strtok(NULL, delimiter);
    }

    for (int i = 0; i < 32; ++i) {
        dir_entry_t *file = &root[i];

        if (file->attributes != 0 || strcmp((char *) file->filename, "") != 0) {
            if (file->attributes == 0) {
                printf("[FOLDER] %s\n", file->filename);
            } else {
                printf("[FILE] %s\n", file->filename);
            }
        }
    }
}
