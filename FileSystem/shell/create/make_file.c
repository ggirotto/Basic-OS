#include <stdio.h>
#include <string.h>
#include "../utils.h"
#include "make_file.h"

/*
    This function creates a file
    Ex: create folder1/file
    Creates file at folder1

*/
void makeFile(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address) {
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
        foundEmptyDirectory->attributes = 1;
    }

    foundEmptyDirectory->first_block = getFreeAddress(fat);

    updateStructures(fatPartition, root, fat, address);
}
