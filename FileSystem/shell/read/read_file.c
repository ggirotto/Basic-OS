#include <stdlib.h>
#include <stdio.h>
#include "../../cluster/data_cluster.h"
#include "../utils.h"

/*
    This function read the content from a file
    Ex: read folder1/file
    Read the content inside file
*/

void readFile(FILE* fatPartition, dir_entry_t* root, uint16_t* address) {
    // Get users input
    char directoryPath[100];
    scanf("%s", directoryPath);

    char *prev = walkThroughPath(directoryPath, fatPartition, root, address);

    if (prev == NULL) {
        return;
    }

    dir_entry_t *foundFileToRead = findEntryNamedAttributed(prev, 1, root);

    if (foundFileToRead == NULL) {
        printf("Arquivo para leitura não existe\n");
        return;
    } else {
        fseek(fatPartition, foundFileToRead->first_block, SEEK_SET);
        char *fileContent = malloc(sizeof(char) * CLUSTER_SIZE );
        fread(fileContent, CLUSTER_SIZE, 1, fatPartition);
        printf("CONTEUDO: %s\n", fileContent);
    }
}
