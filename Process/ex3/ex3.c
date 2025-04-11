#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<signal.h>

void signal_handler(int signal)
{
    if(signal == SIGUSR1)
    {
        printf("Child process ID: %d received SIGUSR1 signal!\n",getpid());
    }
}

int main(int argc, char *argv[])
{
    pid_t pid = fork();
    int status;

    if(pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }else if(pid == 0)
    {
        // child process
        printf("child process ID waiting for signal ...\n");
        signal(SIGUSR1, signal_handler); // Register signal handler
        pause(); // Wait for signal
        printf("Child process ID: %d exiting...\n", getpid());

    }else
    {
        // parrent process
        printf("Parent process ID: %d, Child process ID: %d\n", getpid(), pid);
        sleep(2); // Sleep for 2 seconds
        printf("Parent process sending SIGUSR1 signal to child process ID: %d\n", pid);
        kill(pid, SIGUSR1); // Send signal to child process
        waitpid(pid,&status,0); // Wait for child process to finish
        if(WIFEXITED(status))
        {
            printf("Child process exited with status: %d\n",WIFEXITED(status));
        }else
        {
            printf("Child process terminated abnormally\n");
        }
    }

    return 0;
}