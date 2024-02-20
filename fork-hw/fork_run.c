#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define BUFFER_SIZE 1024

char *getoutput(const char *command) {
    int pipefd[2]; // create a pipe for communication between parent and child
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork(); // fork a child process
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // child process
        dup2(pipefd[1], STDOUT_FILENO); // redirect child's standard output to pipe
        close(pipefd[0]);  // close read end
        close(pipefd[1]);  // close write end
        execl("/bin/sh", "sh", "-c", command, NULL); // execute the command in the child process
        perror("execl");
        exit(EXIT_FAILURE);
    } else { // parent process
        close(pipefd[1]);  // close write end
        char *output = NULL;
        size_t output_size = 0;
        ssize_t bytes_read;
        FILE *pipe_input = fdopen(pipefd[0], "r"); // open pipe for reading
        if (pipe_input == NULL) {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }
        // read from the pipe until '\0' is encountered
        while ((bytes_read = getdelim(&output, &output_size, '\0', pipe_input)) != -1) { // Stack Overflow, "Use of getdelim"
            // continue reading until delimiter '\0' is encountered
        }
        fclose(pipe_input);  // Stack Overflow, "Difference between fclose and close"
        int status;
        waitpid(pid, &status, 0);   
        return output;
    }
}

// Function to execute commands in child processes and collect their output
char *parallelgetoutput(int count, const char **argv_base) {
    int pipefd[2]; // create a pipe for communication between parent and child processes
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    char *combined_output = malloc(1); // allocate memory for combined output
    combined_output[0] = '\0';
    size_t total_output_size = 1;
    for (int i = 0; i < count; i++) { // fork child processes
        pid_t pid = fork(); // fork a child process
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) { // child process
            close(pipefd[0]); // close read end of the pipe
            char index_str[10];  // convert index to string
            snprintf(index_str, sizeof(index_str), "%d", i); // GeeksForGeeks, snprintf()
            const char *argv[20]; // construct argv for execvp
            int j;
            for (j = 0; argv_base[j] != NULL; j++) {
                argv[j] = argv_base[j];
            }
            argv[j++] = index_str;
            argv[j] = NULL;
            dup2(pipefd[1], STDOUT_FILENO); // redirect child's standard output to pipe
            close(pipefd[1]); // close write end of the pipe
            execvp(argv[0], (char * const *)argv); // execute the command in the child process
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    close(pipefd[1]); // close write end of the pipe
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0) { // read from the pipe until end of file
        combined_output = realloc(combined_output, total_output_size + bytes_read); // reallocate memory for combined output
        if (combined_output == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        memcpy(combined_output + total_output_size - 1, buffer, bytes_read); // copy buffer to combined output
        total_output_size += bytes_read; // update total output size
        combined_output[total_output_size - 1] = '\0';  // end with null-terminator
    }
    int status;
    for (int i = 0; i < count; i++) { // wait for all child processes to finish
        wait(&status);
    }
    return combined_output; // return the combined output from all child processes
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
