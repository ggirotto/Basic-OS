#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "dir/dir_entry.h"
#include "cluster/data_cluster.h"
#include "utils.h"

#define CLUSTER_SIZE 1024

uint16_t fat[4096]; // FAT's table 8 clusters, 4096 inputs of 16 bits (8192 bytes)
dir_entry_t root[32];

FILE *fatPartition;

void loadFileSystem() {

    // Load the fat partition
    fatPartition = fopen("fat.part", "r+");
    fseek( fatPartition, CLUSTER_SIZE, SEEK_SET );
    fread( fat, CLUSTER_SIZE, 8, fatPartition );

    fseek( fatPartition, 5120, SEEK_SET );
    fread( root, CLUSTER_SIZE, 1, fatPartition );

}

void initializateFileSystem() {

    // Create the fat partition file
    fatPartition = fopen("fat.part", "w+");

    char initialization = 0xbb;

    for (int i = 0; i < CLUSTER_SIZE; ++i)
    {
        fwrite(&initialization, 1, 1, fatPartition);
    }

    printf("INICIALIZOU BOOT BLOCK\n");

    fat[0] = 0xfffd;

    for (int i = 1; i < 9; ++i)
    {
        fat[i] = 0xfffe;
    }

    fat[9] = 0xffff;

    for (int i = 10; i < 4096; ++i)
    {
        fat[i] = 0x0000;
    }

    fwrite(fat, sizeof(uint16_t), 4096, fatPartition);

    printf("INICIALIZOU FAT\n");

    dir_entry_t rootDir = { 0, 0, 0, 0, 0 };

    for (int i = 0; i < 32; ++i)
    {
        fwrite(&rootDir, sizeof(dir_entry_t), 1, fatPartition);
    }

    printf("INICIALIZOU ROOTDIR\n");

    char dataClusterInitialization = 0x0000;

    for (int i = 0; i < 4086 * CLUSTER_SIZE; ++i)
    {
        fwrite(&dataClusterInitialization, 1, 1, fatPartition);
    }

    printf("INICIALIZOU DATACLUSTERS\n");

}

void listDirectories() {

    // Back to root
    fseek( fatPartition, 9*CLUSTER_SIZE, SEEK_SET );
    fread( root, CLUSTER_SIZE, 1, fatPartition );

    char directoryPath[100];
    scanf("%s", directoryPath);

    char *delimiter = "/";
    char *curr = strtok(directoryPath, delimiter);

    while(curr != NULL) {

        // Walk through paths

        bool foundDirectory = false;

        for (int i = 0; i < 32; ++i)
        {
            dir_entry_t file = root[i];

            if (file.attributes == 0 && strcmp((char *)file.filename, curr) == 0)
            {
                fseek( fatPartition, file.first_block, SEEK_SET );
                fread( root, CLUSTER_SIZE, 1, fatPartition );
                foundDirectory = true;
                break;
            }

        }

        if (!foundDirectory)
        {
            printf("DIRECTORY NOT FOUND \n");
            return;
        }

        curr = strtok(NULL, delimiter);

    }

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t *file = &root[i];

        if (file->attributes != 0 || strcmp((char *)file->filename, "") != 0)
        {
            if (file->attributes == 0)
            {
                printf("[FOLDER] %s\n", file->filename);
            } else {
                printf("[FILE] %s\n", file->filename);
            }
            
        }

    }

}

void makeDirectory() {


    // Back to root
    fseek( fatPartition, 9*CLUSTER_SIZE, SEEK_SET );
    fread( root, CLUSTER_SIZE, 1, fatPartition );

    // Read the path inputed by the user
    char directoryPath[100];
    scanf("%s", directoryPath);

    char *prev;
    char *curr;
    char *delimiter = "/";
    uint16_t address = 9 * CLUSTER_SIZE;

    prev = strtok(directoryPath, delimiter);


    while((curr = strtok(NULL, delimiter)) != NULL) {

        // Walk through paths

        bool foundDirectory = false;

        for (int i = 0; i < 32; ++i)
        {
            dir_entry_t file = root[i];

            if (file.attributes == 0 && strcmp((char *)file.filename, prev) == 0)
            {
                address = file.first_block;
                fseek( fatPartition, file.first_block, SEEK_SET );
                fread( root, CLUSTER_SIZE, 1, fatPartition );
                foundDirectory = true;
            }

        }

        if (!foundDirectory)
        {
            printf("DIRECTORY NOT FOUND \n");
            return;
        }

        prev = curr;

    }

    dir_entry_t *foundEmptyDirectory = NULL;

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t *file = &root[i];

        if (file->attributes == 0 && strcmp((char *)file->filename, "") == 0)
        {
            foundEmptyDirectory = file;
            break;
        }

    }

    if (foundEmptyDirectory == NULL)
    {

        printf("Não tem posição livre no diretório desejado\n");
        return;

    } else
    {

        strcpy((char *)foundEmptyDirectory->filename,prev);

    }

    for (int i = 0; i < 4096; ++i)
    {
        uint16_t address = fat[i];

        if (address == 0x0000)
        {
            
            uint16_t directoryAddress = i * CLUSTER_SIZE;
            printf("Criando essa merda em %d\n", directoryAddress);

            foundEmptyDirectory->first_block = directoryAddress;
            fat[i] = directoryAddress;
            break;

        }

    }

    fseek(fatPartition, CLUSTER_SIZE, SEEK_SET);
    fwrite(fat, 2, 4096, fatPartition);

    fseek(fatPartition, address, SEEK_SET);
    fwrite(root, CLUSTER_SIZE, 1, fatPartition);

}

void makeFile() {


    // Back to root
    fseek( fatPartition, 9*CLUSTER_SIZE, SEEK_SET );
    fread( root, CLUSTER_SIZE, 1, fatPartition );

    char directoryPath[100];

    scanf("%s", directoryPath);

    char *prev;
    char *curr;
    char *delimiter = "/";
    uint16_t address = 9 * CLUSTER_SIZE;

    prev = strtok(directoryPath, delimiter);

    while((curr = strtok(NULL, delimiter)) != NULL) {

        // Walk through paths

        bool foundDirectory = false;

        for (int i = 0; i < 32; ++i)
        {
            dir_entry_t file = root[i];

            if (file.attributes == 0 && strcmp((char *)file.filename, prev) == 0)
            {
                address = file.first_block;
                fseek( fatPartition, file.first_block, SEEK_SET );
                fread( root, CLUSTER_SIZE, 1, fatPartition );
                foundDirectory = true;
            }

        }

        if (!foundDirectory)
        {
            printf("DIRECTORY NOT FOUND \n");
            return;
        }

        prev = curr;

    }

    dir_entry_t *foundEmptyDirectory = NULL;

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t *file = &root[i];

        if (file->attributes == 0 && strcmp((char *)file->filename, "") == 0)
        {
            foundEmptyDirectory = file;
            break;
        }

    }

    if (foundEmptyDirectory == NULL)
    {

        printf("Não tem posição livre no diretório desejado\n");
        return;

    } else
    {

        strcpy((char *)foundEmptyDirectory->filename,prev);
        foundEmptyDirectory->attributes = 1;

    }


    for (int i = 0; i < 4096; ++i)
    {
        uint16_t address = fat[i];

        if (address == 0x0000)
        {
            
            uint16_t directoryAddress = i * CLUSTER_SIZE;
            printf("Criando essa merda em %d\n", directoryAddress);

            foundEmptyDirectory->first_block = directoryAddress;
            fat[i] = directoryAddress;
            break;

        }

    }

    fseek(fatPartition, CLUSTER_SIZE, SEEK_SET);
    fwrite(fat, 2, 4096, fatPartition);

    fseek(fatPartition, address, SEEK_SET);
    fwrite(root, CLUSTER_SIZE, 1, fatPartition);

}


void writeFile() {


    // Back to root
    fseek( fatPartition, 9*CLUSTER_SIZE, SEEK_SET );
    fread( root, CLUSTER_SIZE, 1, fatPartition );

    // Insert user input at directoryPath
    char directoryPath[256];
    fgets (directoryPath, 256, stdin);

    // Create a copy from 'directoryPath' to 'directoryPathContent' to manipulate string content without modify original content
    char directoryPathContent[256];
    snprintf(directoryPathContent, 256, "%s", directoryPath);

    // Get content between "", which the user want to write
    char *delimiter = "\"";
    char *contentToWrite = strtok(directoryPathContent, delimiter);
    contentToWrite = strtok(NULL, delimiter);

    // Remove content between "" from the string.
    char *a = strstr(directoryPath, " ");
    char *b = strrchr(directoryPath, '\"');
    memmove(a-1, b+1, strlen(b)+1);

    char *prev;
    char *curr;
    delimiter = "/";
    uint16_t address = 9 * CLUSTER_SIZE;

    prev = strtok(directoryPath, delimiter);

    while((curr = strtok(NULL, delimiter)) != NULL) {

        // Walk through paths

        bool foundDirectory = false;

        for (int i = 0; i < 32; ++i)
        {
            dir_entry_t file = root[i];

            if (file.attributes == 0 && strcmp((char *)file.filename, prev) == 0)
            {
                address = file.first_block;
                fseek( fatPartition, file.first_block, SEEK_SET );
                fread( root, CLUSTER_SIZE, 1, fatPartition );
                foundDirectory = true;
            }

        }

        if (!foundDirectory)
        {
            printf("DIRECTORY NOT FOUND \n");
            return;
        }

        prev = curr;

    }

    prev[strlen(prev)-1] = 0;

    dir_entry_t *foundFileToWrite = NULL;

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t *file = &root[i];
        if (file->attributes == 1 && strcmp((char *)file->filename, prev) == 0)
        {
            
            foundFileToWrite = file;
            break;
        }

    }

    if (foundFileToWrite == NULL)
    {

        printf("Arquivo para escrita não existe\n");
        return;

    } else
    {

        fseek(fatPartition, foundFileToWrite->first_block, SEEK_SET);
        fwrite(contentToWrite, sizeof(char), CLUSTER_SIZE, fatPartition);

    }

}

/*
   Deal with the user input and decide which file system fuctio to call
*/
void handleUserInput(char userInput[100]) {

    if (strcmp(userInput, "init") == 0) {

        initializateFileSystem();

    } else if (strcmp(userInput, "load") == 0) {

        loadFileSystem();

    } else if (strcmp(userInput, "mkdir") == 0) {
        
        makeDirectory();

    } else if (strcmp(userInput, "write") == 0) {
        
        writeFile();

    } else if (strcmp(userInput, "create") == 0) {
        
        makeFile();

    } else if (strcmp(userInput, "ls") == 0) {
        
        listDirectories();

    } else {
        
        printf("Invalid Command");

    }

}

int main() {

    system("clear");
    printf("File System\n");
    char userInput[100];

    while(strcmp(userInput, "quit") != 0) {

        scanf("%s", userInput);
        handleUserInput(userInput);

    }


    return 0;
}