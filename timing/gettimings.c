#define _XOPEN_SOURCE 700
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

clock_t start_time;

void empty_function() { // scenario 1
    // empty function
}

void sigHandler4(int signum) { // scenario 4
    clock_t end_time = nsecs();
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Elapsed time: %.6f seconds\n", elapsed_time);
}

void sigHandler5(int signum) { // scenario 5
    clock_t end_time = nsecs();
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Elapsed time: %.6f seconds\n", elapsed_time);
}

void send_signal_to_other_process(pid_t pid) { // send signal to another process for scenario 5
    kill(pid, SIGUSR1);
}

long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <scenario_number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int scenario = atoi(argv[1]);
    printf("%lld\n", nsecs());
    switch (scenario) {
        case 1: // scenario 1
            start_time = nsecs();
            empty_function();
            printf("Elapsed time for scenario 1: %.6f seconds\n", ((double)(nsecs() - start_time)) / CLOCKS_PER_SEC);
            break;
        case 2: // scenario 2
            start_time = nsecs();
            getppid();
            printf("Elapsed time for scenario 2: %.6f seconds\n", ((double)(nsecs() - start_time)) / CLOCKS_PER_SEC);
            break;
        case 3: // scenario 3
            start_time = nsecs();
            system("/bin/true");
            printf("Elapsed time for scenario 3: %.6f seconds\n", ((double)(nsecs() - start_time)) / CLOCKS_PER_SEC);
            break;
        case 4: // scenario 4
            signal(SIGUSR1, sigHandler4);
            start_time = nsecs();
            raise(SIGUSR1);
            break;
        case 5: // scenario 5
            signal(SIGUSR1, sigHandler5);
            pid_t other_pid;
            printf("Enter PID of the other process: ");
            scanf("%d", &other_pid);
            start_time = nsecs();
            send_signal_to_other_process(other_pid);
            break;
        default:
            fprintf(stderr, "Invalid scenario number\n");
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}