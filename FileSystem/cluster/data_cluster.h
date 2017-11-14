#ifndef FILESYSTEM_DATA_CLUSTER_H
#define FILESYSTEM_DATA_CLUSTER_H

#include "../dir/dir_entry.h"
#define CLUSTER_SIZE 1024

/*  Directories (ROOT included)
 *   32 directory inputs with 32 bytes each (1024 bytes or data block of 1024 bytes)
 */
union {
    dir_entry_t dir[CLUSTER_SIZE / sizeof(dir_entry_t)];
    uint8_t data[CLUSTER_SIZE];
} data_cluster;

#endif //FILESYSTEM_DATA_CLUSTER_H
