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
uint16_t address;
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


/*
    This function walks through the folders between the root and the final destination.
    Ex: mkdir /folder1/folder2/folder3
    The function walk through folder1, folder2 and update the root array with the
    directory where folder2 is inserted
    Return false if some of the directories inside the path doesnt exists. Othewise, return the last attribute from the path.
    Example: mkdir /folder1/folder2/folder3 -> return folder3 if folder1 and folder2 exists. NULL otherwise.
*/
char* walkThroughPath(char directoryPath[]) {

    address = 9 * CLUSTER_SIZE;

    // Load root directory back at 'root' array.
    fseek( fatPartition, 9*CLUSTER_SIZE, SEEK_SET );
    fread( root, CLUSTER_SIZE, 1, fatPartition );

    char *prev;
    char *curr;
    char *delimiter = "/";

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
            return NULL;
        }

        prev = curr;

    }

    return prev;

}

/*
    This function list all files and folders inside the given path.
    Ex: ls /folder1/folder2
    List all files and folders inside folder2
    For root, type 'ls /'.
*/

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

/*
    Each directory has max 32 slots. If exists a free slot, this function returns a reference to the first one.
*/

dir_entry_t* getFreeEntry() {

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t *file = &root[i];

        if (file->attributes == 0 && strcmp((char *)file->filename, "") == 0)
        {
            return file;
        }

    }

    return NULL;

}

/*
    Find the directory with filename=name
*/

dir_entry_t* findEntryNamed(char* name) {

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t *file = &root[i];

        if (strcmp((char *)file->filename, name) == 0)
        {
            return file;
        }

    }

    return NULL;

}

/*
    Find the directory with filename=name and attributes=attribute
*/

dir_entry_t* findEntryNamedAttributed(char* name, uint8_t attribute) {

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t *file = &root[i];

        if (file->attributes == attribute && strcmp((char *)file->filename, name) == 0)
        {
            return file;
        }

    }

    return NULL;

}

/*
    This function finds the first empty address at fat and set it to an available slot at data clusters.
    Returns the cluster free slot address where the file/folder will be saved.
*/

uint16_t getFreeAddress() {

    for (int i = 0; i < 4096; ++i)
    {
        uint16_t address = fat[i];

        if (address == 0x0000)
        {
            
            uint16_t directoryAddress = i * CLUSTER_SIZE;
            fat[i] = directoryAddress;
            return directoryAddress;

        }

    }

    return 0x0000;

}

/*
    This function update the fat.part file with the data at 'fat' and 'root'.
*/

void updateStructures() {

    fseek(fatPartition, CLUSTER_SIZE, SEEK_SET);
    fwrite(fat, 2, 4096, fatPartition);

    fseek(fatPartition, address, SEEK_SET);
    fwrite(root, CLUSTER_SIZE, 1, fatPartition);

}

/*
    This function creates a folder.
    Ex: mkdir /folder1/folder2
    Creates folder2 inside folder1
*/

void makeDirectory() {

    // Get users input
    char directoryPath[100];
    scanf("%s", directoryPath);

    char *prev = walkThroughPath(directoryPath);

    if (prev == NULL)
    {
        return;
    }

    dir_entry_t *foundEmptyDirectory = getFreeEntry();

    if (foundEmptyDirectory == NULL)
    {

        printf("Não tem posição livre no diretório desejado\n");
        return;

    } else
    {

        strcpy((char *)foundEmptyDirectory->filename,prev);
        foundEmptyDirectory->first_block = getFreeAddress();

        updateStructures();

    }

}

/*
    This function checks if folder is empty
*/
bool checkIfFolderIsEmpty(dir_entry_t *folder) {

    dir_entry_t checkRoot[32];
    fseek( fatPartition, folder->first_block, SEEK_SET );
    fread( checkRoot, CLUSTER_SIZE, 1, fatPartition );

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t file = root[i];

        if (!(file.attributes == 0 && strcmp((char *)file.filename, "") == 0))
        {
            return false;
        }

    }

    return true;

}

void freeDirectoryAddressAtFat(dir_entry_t *directory) {


    for (int i = 0; i < 4096; ++i)
    {
        uint16_t address = fat[i];
        uint16_t directoryAddress = i * CLUSTER_SIZE;

        if (directoryAddress == directory->first_block)
        {
            fat[i] = 0x0000;
            break;
        }

    }

}

void freeDirectoryDataAtRoot(dir_entry_t *directory) {

    for (int i = 0; i < 32; ++i)
    {
        dir_entry_t *file = &root[i];

        if (strcmp((char *)file->filename, (char *)directory->filename) == 0)
        {   
            dir_entry_t emptyDir = { 0, 0, 0, 0, 0 };
            root[i] = emptyDir;
            break;
        }

    }

}

/*
    This function deletes a file or folder.
    Ex: unlink folder1/file
    Delete file inside folder1
    OBS: The folder must be empty to be deleted
*/
void deleteDirectory() {

    // Get users input
    char directoryPath[100];
    scanf("%s", directoryPath);

    char *prev = walkThroughPath(directoryPath);

    if (prev == NULL)
    {
        return;
    }

    dir_entry_t *foundDirectoryToDelete = findEntryNamed(prev);

    if (foundDirectoryToDelete == NULL)
    {

        printf("Diretório/Pasta não encontrado\n");
        return;

    } else if (foundDirectoryToDelete->attributes == 0)
    {

        checkIfFolderIsEmpty(foundDirectoryToDelete);

    }

    char dataClusterInitialization = 0x0000;
    fseek(fatPartition, foundDirectoryToDelete->first_block, SEEK_SET);
    
    fwrite(&dataClusterInitialization, foundDirectoryToDelete->size*sizeof(char), 1, fatPartition);

    freeDirectoryAddressAtFat(foundDirectoryToDelete);
    freeDirectoryDataAtRoot(foundDirectoryToDelete);

    updateStructures();

}

/*
    This function creates a file
    Ex: create folder1/file
    Creates file at folder1

*/
void makeFile() {

    // Get users input
    char directoryPath[100];
    scanf("%s", directoryPath);

    char *prev = walkThroughPath(directoryPath);

    if (prev == NULL)
    {
        return;
    }

    dir_entry_t *foundEmptyDirectory = getFreeEntry();

    if (foundEmptyDirectory == NULL)
    {

        printf("Não tem posição livre no diretório desejado\n");
        return;

    } else
    {

        strcpy((char *)foundEmptyDirectory->filename,prev);
        foundEmptyDirectory->attributes = 1;

    }


    foundEmptyDirectory->first_block = getFreeAddress();

    updateStructures();

}

/*
    This function write content to a file
    Ex: write "something" folder1/file
    Writes 'something' at file inside folder1
*/
void writeFile() {

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

    char *prev = walkThroughPath(directoryPath);

    if (prev == NULL)
    {
        return;
    }

    prev[strlen(prev)-1] = 0;

    dir_entry_t *foundFileToWrite = findEntryNamedAttributed(prev,1);

    if (foundFileToWrite == NULL)
    {

        printf("Arquivo para escrita não existe\n");
        return;

    } else if (sizeof(contentToWrite) > CLUSTER_SIZE) {

        printf("Conteúdo maior que o do cluster\n");
        return;

    } else 
    {

        fseek(fatPartition, foundFileToWrite->first_block, SEEK_SET);
        fwrite(contentToWrite, strlen(contentToWrite)*sizeof(char), 1, fatPartition);
        foundFileToWrite->size = strlen(contentToWrite);

        updateStructures();

    }

}

/*
    This function append content to a file
    Ex: append "something" folder1/file
    Append "something" to the content inside file
*/
void appendFile() {


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

    char *prev = walkThroughPath(directoryPath);

    if (prev == NULL)
    {
        return;
    }

    prev[strlen(prev)-1] = 0;

    dir_entry_t *foundFileToWrite = findEntryNamedAttributed(prev,1);

    if (foundFileToWrite == NULL)
    {

        printf("Arquivo para escrita não existe\n");
        return;

    } else {

        int totalSize = sizeof(contentToWrite) + (foundFileToWrite->size * sizeof(char));

        if ( totalSize > CLUSTER_SIZE ) {

            printf("Conteúdo maior que o do cluster\n");
            return;

        } else {

            uint16_t firstEmptyAddres = foundFileToWrite->first_block + foundFileToWrite->size;
            fseek(fatPartition, firstEmptyAddres, SEEK_SET);
            fwrite(contentToWrite, sizeof(char) * strlen(contentToWrite), 1, fatPartition);
            foundFileToWrite->size += sizeof(contentToWrite);

            updateStructures();
        }
    }

}

/*
    This function read the content from a file
    Ex: read folder1/file
    Read the content inside file
*/

void readFile() {

    // Get users input
    char directoryPath[100];
    scanf("%s", directoryPath);

    char *prev = walkThroughPath(directoryPath);

    if (prev == NULL)
    {
        return;
    }

    dir_entry_t *foundFileToRead = findEntryNamedAttributed(prev,1);

    if (foundFileToRead == NULL)
    {

        printf("Arquivo para leitura não existe\n");
        return;

    } else
    {

        fseek(fatPartition, foundFileToRead->first_block, SEEK_SET);
        char *fileContent = malloc( sizeof(char) * CLUSTER_SIZE );
        fread(fileContent, CLUSTER_SIZE, 1, fatPartition);
        printf("CONTEUDO: %s\n", fileContent);

    }

}

/*
   Deal with the user input and decide which file system function to call
*/
void handleUserInput(char userInput[100]) {

    if (strcmp(userInput, "init") == 0) {

        initializateFileSystem();

    } else if (strcmp(userInput, "load") == 0) {

        loadFileSystem();

    } else if (strcmp(userInput, "mkdir") == 0) {
        
        makeDirectory();

    }
     else if (strcmp(userInput, "unlink") == 0) {
        
        deleteDirectory();

    } else if (strcmp(userInput, "write") == 0) {
        
        writeFile();

    } else if (strcmp(userInput, "append") == 0) {
        
        appendFile();

    } else if (strcmp(userInput, "read") == 0) {
        
        readFile();

    } else if (strcmp(userInput, "create") == 0) {
        
        makeFile();

    } else if (strcmp(userInput, "ls") == 0) {
        
        listDirectories();

    } else if (strcmp(userInput, "quit") == 0) {
        
        exit(0);

    } else if (strcmp(userInput, "clear") == 0) {
        
        system("clear");

    } else {
        
        printf("Invalid Command\n");

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