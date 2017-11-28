#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "../cluster/data_cluster.h"

bool starts_with(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

void wait_for_command() {
    printf("$ ");
}

void clear_screen() {
    system("cls || clear");
    wait_for_command();
}

/*
    This function walks through the folders between the root and the final destination.
    Ex: mkdir /folder1/folder2/folder3
    The function walk through folder1, folder2 and update the root array with the
    directory where folder2 is inserted
    Return false if some of the directories inside the path doesnt exists. Othewise, return the last attribute from the path.
    Example: mkdir /folder1/folder2/folder3 -> return folder3 if folder1 and folder2 exists. NULL otherwise.
*/
char* walkThroughPath(char directoryPath[], FILE* fatPartition, dir_entry_t* root, uint16_t* address) {
    *address = 9 * CLUSTER_SIZE;

    // Load root directory back at 'root' array.
    fseek(fatPartition, 9*CLUSTER_SIZE, SEEK_SET);
    fread(root, CLUSTER_SIZE, 1, fatPartition);

    char *prev;
    char *curr;
    char *delimiter = "/";

    prev = strtok(directoryPath, delimiter);

    while ((curr = strtok(NULL, delimiter)) != NULL) {
        // Walk through paths
        bool foundDirectory = false;

        for (int i = 0; i < 32; ++i) {
            dir_entry_t file = root[i];

            if (file.attributes == 0 && strcmp((char *)file.filename, prev) == 0) {
                *address = file.first_block;
                fseek(fatPartition, file.first_block, SEEK_SET);
                fread(root, CLUSTER_SIZE, 1, fatPartition);
                foundDirectory = true;
            }
        }

        if (!foundDirectory) {
            printf("DIRECTORY NOT FOUND \n");
            return NULL;
        }

        prev = curr;
    }

    return prev;
}

/*
    Each directory has max 32 slots. If exists a free slot, this function returns a reference to the first one.
*/

dir_entry_t* getFreeEntry(dir_entry_t* root) {
    for (int i = 0; i < 32; ++i) {
        dir_entry_t *file = &root[i];

        if (file->attributes == 0 && strcmp((char *)file->filename, "") == 0) {
            return file;
        }
    }

    return NULL;
}

/*
    This function finds the first empty address at fat and set it to an available slot at data clusters.
    Returns the cluster free slot address where the file/folder will be saved.
*/

uint16_t getFreeAddress(uint16_t* fat) {
    for (int i = 0; i < 4096; ++i) {
        uint16_t address = fat[i];

        if (address == 0x0000) {
            uint16_t directoryAddress = i * CLUSTER_SIZE;
            fat[i] = directoryAddress;
            return directoryAddress;
        }
    }

    return 0x0000;
}

/*
    This function update the fat.part file with the data at 'fat' and 'root'.
*/

void updateStructures(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address) {
    fseek(fatPartition, CLUSTER_SIZE, SEEK_SET);
    fwrite(fat, 2, 4096, fatPartition);

    fseek(fatPartition, *address, SEEK_SET);
    fwrite(root, CLUSTER_SIZE, 1, fatPartition);
}

/*
    Find the directory with filename=name
*/

dir_entry_t* findEntryNamed(char* name, dir_entry_t* root) {
    for (int i = 0; i < 32; ++i) {
        dir_entry_t *file = &root[i];

        if (strcmp((char *)file->filename, name) == 0) {
            return file;
        }
    }

    return NULL;
}

/*
    This function checks if folder is empty
*/
bool checkIfFolderIsEmpty(dir_entry_t *folder, FILE* fatPartition, dir_entry_t* root) {
    dir_entry_t checkRoot[32];
    fseek(fatPartition, folder->first_block, SEEK_SET);
    fread(checkRoot, CLUSTER_SIZE, 1, fatPartition);

    for (int i = 0; i < 32; ++i) {
        dir_entry_t file = root[i];

        if (!(file.attributes == 0 && strcmp((char *)file.filename, "") == 0)) {
            return false;
        }
    }

    return true;
}


void freeDirectoryAddressAtFat(dir_entry_t *directory, uint16_t* fat) {
    for (int i = 0; i < 4096; ++i) {
        uint16_t address = fat[i];
        uint16_t directoryAddress = i * CLUSTER_SIZE;

        if (directoryAddress == directory->first_block) {
            fat[i] = 0x0000;
            break;
        }
    }
}

void freeDirectoryDataAtRoot(dir_entry_t *directory, dir_entry_t* root) {
    for (int i = 0; i < 32; ++i) {
        dir_entry_t *file = &root[i];

        if (strcmp((char *)file->filename, (char *)directory->filename) == 0) {
            dir_entry_t emptyDir = { 0, 0, 0, 0, 0 };
            root[i] = emptyDir;
            break;
        }
    }
}


/*
    Find the directory with filename=name and attributes=attribute
*/

dir_entry_t* findEntryNamedAttributed(char* name, uint8_t attribute, dir_entry_t* root) {
    for (int i = 0; i < 32; ++i) {
        dir_entry_t *file = &root[i];

        if (file->attributes == attribute && strcmp((char *)file->filename, name) == 0) {
            return file;
        }
    }

    return NULL;
}
