#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int my_system(const char *command) {
    if (command == NULL) {
        return 1;
    }
    pid_t pid;
    int status;
    pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        execlp("sh", "sh", "-c", command, (char *)NULL);
        exit(EXIT_FAILURE);
    } else {
        if (waitpid(pid, &status, 0) == -1) {
            return -1;
        }
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1; 
        }
    }
}