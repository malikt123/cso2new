#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
char **string_split(const char *input, const char *sep, int *num_words) {
    int input_start_forbidden = (strchr(sep, input[0]) != NULL);
    int count = 0;
    const char *ptr = input;
    while (*ptr != '\0') {
        while (*ptr != '\0' && strchr(sep, *ptr) != NULL) {
            ptr++;
        }
        if (*ptr != '\0') {
            count++;
        }
        while (*ptr != '\0' && strchr(sep, *ptr) == NULL) {
            ptr++;
        }
    }
    char **words = (char **)malloc((count + input_start_forbidden + 1) * sizeof(char *));
    if (words == NULL) {
        *num_words = 0;
        return NULL;
    }
    int index = input_start_forbidden;
    if (input_start_forbidden) {
        words[0] = (char *)malloc(1 * sizeof(char));
        if (words[0] == NULL) {
            free(words);
            *num_words = 0;
            return NULL;
        }
        words[0][0] = '\0';
    }
    ptr = input;
    while (*ptr != '\0') {
        while (*ptr != '\0' && strchr(sep, *ptr) != NULL) {
            ptr++;
        }
        if (*ptr == '\0') {
            break;
        }
        const char *start = ptr;
        while (*ptr != '\0' && strchr(sep, *ptr) == NULL) {
            ptr++;
        }
        int word_length = ptr - start;
        words[index] = (char *)malloc((word_length + 1) * sizeof(char));
        if (words[index] == NULL) {
            for (int i = 0; i < index; i++) {
                free(words[i]);
            }
            free(words);
            *num_words = 0;
            return NULL;
        }
        strncpy(words[index], start, word_length);
        words[index][word_length] = '\0';
        index++;
    }
    words[index] = NULL;
    *num_words = index;
    return words;
}
