#ifndef FILESYSTEM_MAKE_DIRECTORY_H
#define FILESYSTEM_MAKE_DIRECTORY_H

#include "../../dir/dir_entry.h"

void makeDirectory(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address);

#endif //FILESYSTEM_MAKE_DIRECTORY_H
