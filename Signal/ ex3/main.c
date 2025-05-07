#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Sjgnal handler for SIGUSR1
void signal_handler(int sig) {
    printf("Received signal from parent\n");
}

int main() {
    pid_t pid;
    int count = 0;

    // Ceate chil process
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) { // Child process
        // Register signal handler for SIGUSR1
        signal(SIGUSR1, signal_handler);
        
        // Keep chil running to receive signals
        while (1) {
            pause(); // Wait for signals
        }
    } else { // Parent process
        // Send SIGUSR1 to child every 2 seconds, 5 times
        while (count < 5) {
            sleep(2);
            kill(pid, SIGUSR1);
            printf("Parent sent SIGUSR1 to child\n");
            count++;
        }
        
        // Terminate child process
        kill(pid, SIGTERM);
        wait(NULL); // Wait for child to terminate
        printf("Program terminated\n");
    }

    return 0;
}
