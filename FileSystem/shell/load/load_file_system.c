#include "stdio.h"
#include "load_file_system.h"
#include "../../cluster/data_cluster.h"


FILE* loadFileSystem(uint16_t* fat, dir_entry_t* root) {
    // Load the fat partition
    FILE* fatPartition;
    fatPartition = fopen("fat.part", "r+");
    fseek(fatPartition, CLUSTER_SIZE, SEEK_SET);
    fread(fat, CLUSTER_SIZE, 8, fatPartition);

    fseek(fatPartition, 5120, SEEK_SET);
    fread(root, CLUSTER_SIZE, 1, fatPartition);

    return fatPartition;
}
