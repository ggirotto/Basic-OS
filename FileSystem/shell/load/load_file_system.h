#ifndef FILESYSTEM_LOAD_FILE_SYSTEM_H
#define FILESYSTEM_LOAD_FILE_SYSTEM_H

#include <stdio.h>
#include <stdint.h>
#include "../../dir/dir_entry.h"


FILE* loadFileSystem(uint16_t* fat, dir_entry_t* root);

#endif //FILESYSTEM_LOAD_FILE_SYSTEM_H
