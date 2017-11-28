#include <stdio.h>
#include "delete_directory.h"
#include "../utils.h"

/*
    This function deletes a file or folder.
    Ex: unlink folder1/file
    Delete file inside folder1
    OBS: The folder must be empty to be deleted
*/

void deleteDirectory(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address) {
    // Get users input
    char directoryPath[100];
    scanf("%s", directoryPath);

    char *prev = walkThroughPath(directoryPath, fatPartition, root, address);

    if (prev == NULL) {
        return;
    }

    dir_entry_t *foundDirectoryToDelete = findEntryNamed(prev, root);

    if (foundDirectoryToDelete == NULL) {
        printf("Diretório/Pasta não encontrado\n");
        return;
    } else if (foundDirectoryToDelete->attributes == 0) {
        checkIfFolderIsEmpty(foundDirectoryToDelete, fatPartition, root);
    }

    freeDirectoryAddressAtFat(foundDirectoryToDelete, fat);
    freeDirectoryDataAtRoot(foundDirectoryToDelete, root);

    updateStructures(fatPartition, root, fat, address);
}
