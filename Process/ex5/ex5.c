#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char* argv[])
{

    pid_t pid_orphan = fork(); // Create a new process
    int status;

    if(pid_orphan < 0)
    {
        // Fork failed
        perror("Fork failed");
        return 1;

    }else if(pid_orphan == 0)
    {
        // Child process --> Orphan process
        printf("Orphan process ID: %d\n", getpid());
        sleep(15); // Sleep for 15 seconds to allow the parent process to terminate
        printf("Orphan process ID: %d\n Exiting\n", getpid());
        exit(0);

    }else
    {
        // parent process
        pid_t pid_zombies = fork(); // Create a new process
        if(pid_zombies < 0)
        {
            // Fork failed
            perror("Fork failed");
            return 1;
        }else if(pid_zombies == 0)
        {
            // Child process --> Zombie process
            printf("Zombie process ID: %d\n", getpid());
            sleep(5); // Sleep for 5 seconds to allow the parent process to terminate
            printf("Zombie process ID: %d Exiting\n", getpid());
            exit(0);
        }else
        {
            // Parent process
            printf("Parent process PID: %d, Child process PID: %d\n", getpid(), pid_zombies);
            sleep(10); // Sleep for 10 seconds to allow the child process to become a zombie
            wait(&status); // Wait for the child process to terminate
            if(WIFEXITED(status))
            {
                printf("Child process exited with status: %d\n", WEXITSTATUS(status));
            }else 
            {
                printf("Child process terminated abnormally\n");
            }
        }

    }

    return 0;
}