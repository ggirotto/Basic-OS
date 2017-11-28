#include <stdio.h>
#include <string.h>
#include "list_directories.h"


/*
    This function list all files and folders inside the given path.
    Ex: ls /folder1/folder2
    List all files and folders inside folder2
    For root, type 'ls /'.
*/

void listDirectories(FILE* fatPartition, dir_entry_t* root, uint16_t* address) {
    // Get users input
    char directoryPath[100];
    scanf("%s", directoryPath);

    walkThroughPath(directoryPath, fatPartition, root, address);

    /*
        For each file that is not an empty file, print it's name and it's type
    */

    for (int i = 0; i < 32; ++i) {
        dir_entry_t *file = &root[i];

        if (file->attributes != 0 || strcmp((char *)file->filename, "") != 0) {
            if (file->attributes == 0) {
                printf("[FOLDER] %s\n", file->filename);
            } else {
                printf("[FILE] %s\n", file->filename);
            }
        }
    }
}
