#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_SIZE 4096  

pid_t other_pid;  
char *inbox = NULL;  
char *outbox = NULL;  
char inbox_name[256], outbox_name[256];  

// Function to handle incoming messages and termination signals
void signal_handler(int sig) {
    if (sig == SIGUSR1 && inbox[0] != '\0') {
        printf("\nReceived message: %s\n", inbox);
        fflush(stdout);
        inbox[0] = '\0';  // Clears
    } else if (sig == SIGINT || sig == SIGTERM) {
        // Send SIGTERM to the other process to instruct it to terminate
        if (other_pid > 0) {
            kill(other_pid, SIGTERM);
        }

        if (inbox != NULL) {
            munmap(inbox, SHM_SIZE);
            shm_unlink(inbox_name);  // Remove the shared memory object for inbox
        }
        if (outbox != NULL) {
            munmap(outbox, SHM_SIZE);
            shm_unlink(outbox_name);  // Remove the shared memory object for outbox
        }

        exit(0); //indicate successful termination
    }
}

// Setup signal handler and shared memory
void setup(pid_t other_pid) {
    // Signal handler setup for SIGUSR1, SIGINT, and SIGTERM
    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Creating names for shared memory objects
    snprintf(inbox_name, sizeof(inbox_name), "/%d_inbox", getpid());
    snprintf(outbox_name, sizeof(outbox_name), "/%d_inbox", other_pid);

    // Open and map inbox
    int inbox_fd = shm_open(inbox_name, O_CREAT | O_RDWR, 0600);
    ftruncate(inbox_fd, SHM_SIZE);
    inbox = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, inbox_fd, 0);
    close(inbox_fd);

    // Open and map outbox (connecting to other process's inbox)
    int outbox_fd = shm_open(outbox_name, O_CREAT | O_RDWR, 0600);
    ftruncate(outbox_fd, SHM_SIZE);
    outbox = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, outbox_fd, 0);
    close(outbox_fd);

    // Initially clear the inbox
    inbox[0] = '\0';
}

int main() {
    printf("This process's ID: %ld\n", (long)getpid());
    printf("Enter other process ID: ");
    scanf("%d", &other_pid);
    getchar(); // Clear the newline character left by scanf

    setup(other_pid);

    while (1) {
        printf("Enter message: ");
        char message[SHM_SIZE];
        fgets(message, SHM_SIZE, stdin);
        strncpy(outbox, message, SHM_SIZE);
        kill(other_pid, SIGUSR1);  // Notify the other process
    }

    return 0;  // Cleanup will be handled by signal_handler
}
