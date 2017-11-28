#ifndef FILESYSTEM_LIST_DIRECTORIES_H
#define FILESYSTEM_LIST_DIRECTORIES_H

#include <stdio.h>
#include "../utils.h"

void listDirectories(FILE* fatPartition, dir_entry_t* root, uint16_t* address);

#endif //FILESYSTEM_LIST_DIRECTORIES_H
