/*
**==============================================================================
**
** Copyright (c) Microsoft Corporation. All rights reserved. See file LICENSE
** for license information.
**
**==============================================================================
*/

#include <stdlib.h>
#include <MI.h>

#include <stdio.h>
#include <string.h>

int climain();

int main(int argc, const char *argv[]) {
    // Check if filename is provided as an argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Open the file specified in the command line
    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open the file");
        return 1;
    }

    // Buffer to store the entire line from the file
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Read the entire line (only one line expected)
    if ((read = getline(&line, &len, file)) == -1) {
        perror("Failed to read from file");
        free(line);
        fclose(file);
        return 1;
    }

    // Close the file after reading
    fclose(file);

    // Remove the newline character at the end if it's there
    if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
    }

    // Estimate the number of arguments by counting spaces
    int climain_argc = 0;
    for (int i = 0; i < read; i++) {
        if (line[i] == ' ') {
            climain_argc++;
        }
    }
    // Adding one to climain_argc for the last argument (as there is no space after the last word)
    climain_argc++;

    // Allocate memory for the argv array
    const char **climain_argv = malloc(sizeof(char*) * climain_argc);
    if (climain_argv == NULL) {
        perror("Memory allocation failed");
        free(line);
        return 1;
    }

    // Tokenize the line by spaces and fill climain_argv
    int index = 0;
    char *token = strtok(line, " ");
    while (token != NULL) {
        climain_argv[index] = strdup(token);  // Copy each token
        if (climain_argv[index] == NULL) {
            perror("Memory allocation failed for token");
            free(line);
            for (int i = 0; i < index; i++) {
                free((void*)climain_argv[i]);
            }
            free(climain_argv);
            return 1;
        }
        index++;
        token = strtok(NULL, " ");
    }

    // Free the line buffer since we don't need it anymore
    free(line);

    // Call climain with the generated argc and argv
    int result = climain(climain_argc, climain_argv);

    // Free the memory allocated for arguments
    for (int i = 0; i < climain_argc; i++) {
        free((void*)climain_argv[i]);  // Free each strdup'ed argument
    }
    free(climain_argv);  // Free the argv array itself

    return result;
}



int MI_MAIN_CALL main2(int argc, const char* argv[])
{
#if defined (MI_USE_WCHAR)
    /* Going to need to convert the args to MI_Char */
    printf("Do not support %s when using wide char support yet\n", argv[0]);
    return 0;
#else
    exit((int) climain(argc, argv));
#endif
}
