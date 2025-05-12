#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork(); // Create a new process

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // This block is executed by the child process
        printf("Child process PID: %d\n", getpid());
    } else {
        // This block is executed by the parent process
        printf("Parent process PID: %d, Child process PID: %d\n", getpid(), pid);
    }

    return 0;
}
