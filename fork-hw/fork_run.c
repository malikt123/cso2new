#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define BUFFER_SIZE 1024

char *getoutput(const char *command) {
    int pipefd[2]; // make pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // child  process
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);  // close read end
        close(pipefd[1]);  // close write end

        execl("/bin/sh", "sh", "-c", command, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // parent process
        close(pipefd[1]);  // close write end
        char *output = NULL;
        size_t output_size = 0;
        ssize_t bytes_read;
        FILE *pipe_input = fdopen(pipefd[0], "r"); // Stack Overflow, "Difference between fclose and close"
        if (pipe_input == NULL) {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }
        while ((bytes_read = getdelim(&output, &output_size, '\0', pipe_input)) != -1) { // Stack Overflow, "Use of getdelim"
            // read until '\0' is encountered
        }
        fclose(pipe_input);  // close read end
        int status;
        waitpid(pid, &status, 0);   
        return output;
    }
}

char *parallelgetoutput(int count, const char **argv_base) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    char *combined_output = malloc(1); // malloc space
    combined_output[0] = '\0';
    size_t total_output_size = 1;
    for (int i = 0; i < count; i++) {
        pid_t pid = fork(); // fork count amt of child processes
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            close(pipefd[0]); // close read end
            char index_str[10];  // make space for 10 digits
            snprintf(index_str, sizeof(index_str), "%d", i); // GeeksForGeeks, snprintf()
            const char *argv[20];  // make space for 20 args
            int j;
            for (j = 0; argv_base[j] != NULL; j++) {
                argv[j] = argv_base[j];
            }
            argv[j++] = index_str;
            argv[j] = NULL;
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            execvp(argv[0], (char * const *)argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    close(pipefd[1]);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
        combined_output = realloc(combined_output, total_output_size + bytes_read);
        if (combined_output == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        memcpy(combined_output + total_output_size - 1, buffer, bytes_read);
        total_output_size += bytes_read;
        combined_output[total_output_size - 1] = '\0';  // end with null-terminator
    }
    int status;
    for (int i = 0; i < count; i++) {
        wait(&status);
    }
    return combined_output;
}

// int main() {
//     const char *argv_base[] = { "/bin/echo", "running", NULL };
//     const char *output = parallelgetoutput(2, argv_base);
//     printf("Text: [%s]\n", output);
//     free((void *)output);  // free memory
//     return 0;
// }

// int main() {
//     printf("Hi!\n");
//     char *result = getoutput("echo 1 2 3; sleep 2; echo 5 5");
//     printf("Text: [[[%s]]]\n", result);
//     printf("Bye!\n");
//     free(result);
//     return 0;
// }
