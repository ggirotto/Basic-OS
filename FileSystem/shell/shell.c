#include <stdio.h>
#include <stdint.h>
#include "../dir/dir_entry.h"
#include "utils.h"
#include "init/init_file_system.h"
#include "load/load_file_system.h"
#include "mkdir/make_directory.h"
#include "unlink/delete_directory.h"
#include "write/write_file.h"
#include "append/append_file.h"
#include "ls/list_directories.h"
#include "create/make_file.h"
#include "read/read_file.h"

uint16_t fat[4096]; // FAT's table 8 clusters, 4096 inputs of 16 bits (8192 bytes)
dir_entry_t root[32];
FILE* fatPartition;
uint16_t address;

/*
   Deal with the user input and decide which file system function to call
*/
void handleUserInput(const char* userInput) {
    if (starts_with(userInput, "init")) {
        fatPartition = initializateFileSystem(fat);
    } else if (starts_with(userInput, "load")) {
        fatPartition = loadFileSystem(fat, root);
    } else if (starts_with(userInput, "mkdir")) {
        makeDirectory(fatPartition, root, fat, &address);
    } else if (starts_with(userInput, "unlink")) {
        deleteDirectory(fatPartition, root, fat, &address);
    } else if (starts_with(userInput, "write")) {
        writeFile(fatPartition, root, fat, &address);
    } else if (starts_with(userInput, "append")) {
        appendFile(fatPartition, root, fat, &address);
    } else if (starts_with(userInput, "read")) {
        readFile(fatPartition, root, &address);
    } else if (starts_with(userInput, "create")) {
        makeFile(fatPartition, root, fat, &address);
    } else if (starts_with(userInput, "ls")) {
        listDirectories(fatPartition, root, &address);
    } else if (starts_with(userInput, "clear")) {
        clear_screen();
    } else if (starts_with(userInput, "exit")) {
        return;
    } else {
        printf("Invalid Command\n");
    }

    wait_for_command();
}
