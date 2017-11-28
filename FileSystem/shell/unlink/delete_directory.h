#ifndef FILESYSTEM_DELETE_DIRECTORY_H
#define FILESYSTEM_DELETE_DIRECTORY_H

#include <stdio.h>
#include "../../dir/dir_entry.h"

void deleteDirectory(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address);

#endif //FILESYSTEM_DELETE_DIRECTORY_H
