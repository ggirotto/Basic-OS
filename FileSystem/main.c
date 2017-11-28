#include <stdio.h>
#include "shell/utils.h"
#include "shell/shell.h"

int main() {
    clear_screen();

    char userInput[100];
    const char* user_input_ptr = userInput;

    while (!starts_with(user_input_ptr, "exit")) {
        scanf("%s", userInput);
        handleUserInput(userInput);
    }

    return 0;
}