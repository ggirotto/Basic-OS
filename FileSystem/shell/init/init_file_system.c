#include <stdio.h>
#include "init_file_system.h"
#include "../../cluster/data_cluster.h"

FILE* initializateFileSystem(uint16_t* fat) {
    // Create the fat partition file
    FILE* fatPartition;
    fatPartition = fopen("fat.part", "w+");

    char initialization = 0xbb;

    for (int i = 0; i < CLUSTER_SIZE; ++i) {
        fwrite(&initialization, 1, 1, fatPartition);
    }

    printf("INICIALIZOU BOOT BLOCK\n");

    fat[0] = 0xfffd;

    for (int i = 1; i < 9; ++i) {
        fat[i] = 0xfffe;
    }

    fat[9] = 0xffff;

    for (int i = 10; i < 4096; ++i) {
        fat[i] = 0x0000;
    }

    fwrite(fat, sizeof(uint16_t), 4096, fatPartition);

    printf("INICIALIZOU FAT\n");

    dir_entry_t rootDir = { 0, 0, 0, 0, 0 };

    for (int i = 0; i < 32; ++i) {
        fwrite(&rootDir, sizeof(dir_entry_t), 1, fatPartition);
    }

    printf("INICIALIZOU ROOTDIR\n");

    char dataClusterInitialization = 0x0000;

    for (int i = 0; i < 4086 * CLUSTER_SIZE; ++i) {
        fwrite(&dataClusterInitialization, 1, 1, fatPartition);
    }

    printf("INICIALIZOU DATACLUSTERS\n");

    return fatPartition;
}
