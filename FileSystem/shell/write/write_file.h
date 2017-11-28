#ifndef FILESYSTEM_WRITE_FILE_H
#define FILESYSTEM_WRITE_FILE_H

#include <stdio.h>
#include "../../dir/dir_entry.h"

void writeFile(FILE* fatPartition, dir_entry_t* root, uint16_t* fat, uint16_t* address);

#endif //FILESYSTEM_WRITE_FILE_H
