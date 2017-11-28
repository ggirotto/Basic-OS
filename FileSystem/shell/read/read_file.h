#ifndef FILESYSTEM_READ_FILE_H
#define FILESYSTEM_READ_FILE_H

#include <stdio.h>
#include "../../dir/dir_entry.h"

void readFile(FILE* fatPartition, dir_entry_t* root, uint16_t* address);

#endif //FILESYSTEM_READ_FILE_H
