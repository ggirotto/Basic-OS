#include <stdio.h>
#include <string.h>
#include "make_directory.h"
#include "../utils.h"

/*
    This function creates a folder.
    Ex: mkdir /folder1/folder2
    Creates folder2 inside folder1
*/
void makeDirectory(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address) {
    // Get users input
    char directoryPath[100];
    scanf("%s", directoryPath);

    char *prev = walkThroughPath(directoryPath, fatPartition, root, address);

    if (prev == NULL) {
        return;
    }

    dir_entry_t *foundEmptyDirectory = getFreeEntry(root);

    if (foundEmptyDirectory == NULL) {
        printf("Não tem posição livre no diretório desejado\n");
        return;

    } else {
        strcpy((char *)foundEmptyDirectory->filename,prev);
        foundEmptyDirectory->first_block = getFreeAddress(fat);

        updateStructures(fatPartition, root, fat, address);
    }
}
