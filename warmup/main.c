#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split.h"
#define MAX_INPUT_LENGTH 4000

char *strdup(const char *s);
char *concat_args(int argc, char *argv[]) {
    int total_length = 0;
    for (int i = 1; i < argc; i++) {
        total_length += strlen(argv[i]);
    }
    char *sep = (char *)malloc((total_length + argc - 2 + 1) * sizeof(char));
    if (sep == NULL) {
        fprintf(stderr, "Could not allocate memory\n");
        exit(EXIT_FAILURE);
    }
    sep[0] = '\0';
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
        if (strcmp(input, ".\n") == 0) {
            break;
        }
        char *newline = strchr(input, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }
        int num_words;
        char **words = string_split(input, sep, &num_words);
        if (words == NULL) {
            fprintf(stderr, "Error splitting input\n");
            exit(EXIT_FAILURE);
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
