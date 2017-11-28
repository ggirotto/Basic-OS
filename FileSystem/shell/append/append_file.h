#ifndef FILESYSTEM_APPEND_FILE_H
#define FILESYSTEM_APPEND_FILE_H

#include <stdio.h>
#include "../../dir/dir_entry.h"

void appendFile(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address);

#endif //FILESYSTEM_APPEND_FILE_H
