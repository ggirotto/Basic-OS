#ifndef FILESYSTEM_UTILS_H
#define FILESYSTEM_UTILS_H

#include <stdbool.h>
#include "../dir/dir_entry.h"

bool starts_with(const char *pre, const char *str);

void wait_for_command();

void clear_screen();

char* walkThroughPath(char directoryPath[], FILE* fatPartition, dir_entry_t* root, uint16_t* address);

dir_entry_t* getFreeEntry(dir_entry_t* root);

uint16_t getFreeAddress(uint16_t* fat);

void updateStructures(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address);

dir_entry_t* findEntryNamed(char* name, dir_entry_t* root);

bool checkIfFolderIsEmpty(dir_entry_t *folder, FILE* fatPartition, dir_entry_t* root);

void freeDirectoryAddressAtFat(dir_entry_t *directory, uint16_t* fat);

void freeDirectoryDataAtRoot(dir_entry_t *directory, dir_entry_t* root);

dir_entry_t* findEntryNamedAttributed(char* name, uint8_t attribute, dir_entry_t* root);

#endif //FILESYSTEM_UTILS_H
