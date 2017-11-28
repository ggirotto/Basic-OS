#ifndef FILESYSTEM_MAKE_FILE_H
#define FILESYSTEM_MAKE_FILE_H

#include <stdio.h>
#include "../../dir/dir_entry.h"

void makeFile(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address);

#endif //FILESYSTEM_MAKE_FILE_H
