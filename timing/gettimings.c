#define _XOPEN_SOURCE 700
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


clock_t start_time;
pid_t other_pid;
int NUM_ITERATIONS = 100;

long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}

void empty_function() { // scenario 1
    // empty function
}

void sigHandler4(int signum) { // scenario 4
    clock_t end_time = nsecs();
    double elapsed_time = ((double) (end_time - start_time));
    printf("Elapsed time for scenario 4: %.6f nanoseconds\n", elapsed_time);
}

void sigHandler5(int signum) { // scenario 5
    // empty
}

void sigHandler5Response(int signum) { // scenario 5
    kill(other_pid, SIGUSR1);
}

void send_signal_to_other_process(int signum) { // send signal to another process for scenario 5
    kill(other_pid, SIGUSR1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <scenario_number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int scenario = atoi(argv[1]);
    double total_elapsed_time = 0.0;
    switch (scenario) {
        case 1: // scenario 1
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                start_time = nsecs();
                empty_function();
                total_elapsed_time += ((double)(nsecs() - start_time));
            }
            printf("Average elapsed time for scenario 1 (100 iterations): %lld nanoseconds\n", (long long int) total_elapsed_time / NUM_ITERATIONS);
            break;
        case 2: // scenario 2
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                start_time = nsecs();
                getppid();
                total_elapsed_time += ((double)(nsecs() - start_time));
            }
            printf("Average elapsed time for scenario 2 (100 iterations): %lld nanoseconds\n", (long long int) total_elapsed_time / NUM_ITERATIONS);
            break;
        case 3: // scenario 3
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                start_time = nsecs();
                system("/bin/true");
                total_elapsed_time += ((double)(nsecs() - start_time));
            }
            printf("Average elapsed time for scenario 3 (100 iterations): %lld nanoseconds\n", (long long int) total_elapsed_time / NUM_ITERATIONS);
            break;
        case 4: // scenario 4
        {
            struct sigaction sa4;
            sa4.sa_handler = &sigHandler4;
            sigemptyset(&sa4.sa_mask);
            sa4.sa_flags = SA_RESTART;
            sigaction(SIGUSR1, &sa4, NULL);
            signal(SIGUSR1, SIG_IGN);
            
            double total_elapsed_time = 0.0;
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                start_time = nsecs();
                raise(SIGUSR1);
                total_elapsed_time += ((double)(nsecs() - start_time));
            }
            double average_elapsed_time = total_elapsed_time / NUM_ITERATIONS;
            printf("Average elapsed time for scenario 4 (100 iterations): %lld nanoseconds\n", (long long int) average_elapsed_time);
        }
        break;
        case 5: // scenario 5
        {
            printf("Current process PID: %d\n", getpid());
            printf("Enter PID of the other process: ");
            scanf("%d", &other_pid);
            struct sigaction sa5;
            sa5.sa_handler = &sigHandler5;
            sigemptyset(&sa5.sa_mask);
            sa5.sa_flags = SA_RESTART;
            sigaction(SIGUSR1, &sa5, NULL);
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            sigprocmask(SIG_BLOCK, &mask, NULL);
            for (int i = 0; i < NUM_ITERATIONS; i++) {
                start_time = nsecs();
                kill(other_pid, SIGUSR1);
                int sig;
                sigwait(&mask, &sig);
                total_elapsed_time += ((double)(nsecs() - start_time));
            }
            total_elapsed_time = total_elapsed_time;
            double average_elapsed_time = total_elapsed_time / NUM_ITERATIONS;
            printf("Average elapsed time for scenario 5 (100 iterations): %lld nanoseconds\n", (long long int) average_elapsed_time);
        }
        break;
        case -1: // scenario -1
        {
            struct sigaction sa5;
            sa5.sa_handler = &sigHandler5Response;
            sigemptyset(&sa5.sa_mask);
            sa5.sa_flags = SA_RESTART;
            sigaction(SIGUSR1, &sa5, NULL);
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            sigprocmask(SIG_BLOCK, &mask, NULL);
            printf("Current process PID: %d\n", getpid()); // Print PID
            printf("Enter PID of the process running scenario 5: ");
            scanf("%d", &other_pid);
            while(1){
                int sig;
                sigwait(&mask, &sig);
                kill(other_pid, SIGUSR1);
            }
        }
        break;
        default:
            fprintf(stderr, "Invalid scenario number\n");
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}