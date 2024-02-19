#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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
        FILE *pipe_input = fdopen(pipefd[0], "r");
        if (pipe_input == NULL) {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }
        while ((bytes_read = getdelim(&output, &output_size, '\0', pipe_input)) != -1) {
            // read until '\0' is encountered
        }
        fclose(pipe_input);  // close read end
        int status;
        waitpid(pid, &status, 0);   
        return output;
    }
}

// int main() {
//     printf("Hi!\n");
//     char *result = getoutput("echo 1 2 3; sleep 2; echo 5 5");
//     printf("Text: [[[%s]]]\n", result);
//     printf("Bye!\n");
//     free(result);
//     return 0;
// }
