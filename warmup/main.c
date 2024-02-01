#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split.h"
#include <ctype.h>

#define MAX_INPUT_LENGTH 4000

char *concat_args(int argc, char *argv[]) {
    int total_length = 0;
    for (int i = 1; i < argc; i++) {
        total_length += strlen(argv[i]);
    }
    char *sep = (char *)malloc((total_length + argc - 2 + 1) * sizeof(char)); // +1 for null terminator
    if(argc == 1){
        sep = " \t";
        return sep;
    } 
    if (sep == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    sep[0] = '\0'; // Initialize with null terminator
    for (int i = 1; i < argc; i++) {
        strcat(sep, argv[i]);
    }
    return sep;
}

int main(int argc, char *argv[]) {
    char *sep;
    if (argc > 1) {
        sep = concat_args(argc, argv);
    } else {
        sep = strdup(" \t");
        if (sep == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }

    char input[MAX_INPUT_LENGTH];
    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            exit(EXIT_FAILURE);
        }
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, ".\n") == 0) {
            break;
        }

        int confiscated_beginning = 0;

        // Check if the first character is a forbidden character
        if (isspace(input[0])) {
            confiscated_beginning = 1;
        }

        char **words;
        int num_words;  
        if (strlen(input) == 1 && input[0] == '.') {
            break;
        }
        words = string_split(input, sep, &num_words);
        if (words == NULL) {
            fprintf(stderr, "Error splitting input\n");
            exit(EXIT_FAILURE);
        }

        if (confiscated_beginning) {
            // Prepend an empty string to the result array
            char **new_words = (char **)malloc((num_words + 2) * sizeof(char *));
            if (new_words == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            new_words[0] = (char *)malloc(1 * sizeof(char));
            if (new_words[0] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            new_words[0][0] = '\0'; // Add empty string
            memcpy(&new_words[1], words, (num_words + 1) * sizeof(char *));
            free(words);
            words = new_words;
            num_words++;
        }

        for (int i = 0; i < num_words; i++) {
            printf("[%s]", words[i]);
            free(words[i]);
        }
        printf("\n");

        free(words);
    }

    free(sep);

    return 0;
}