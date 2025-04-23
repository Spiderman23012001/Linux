#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int a = 1;

int main(int argc, char* argv[])
{

    pid_t child_process = fork(); // Create a new process

    if(child_process < 0)
    {
        // Fork failed
        perror("Fork failed");
        return 1;

    }else if(child_process == 0)
    {
        // Child process --> Orphan process
        a = 3;
        printf("Child process: %d\n",child_process);
        printf("a = %d\n",a);
        exit(0);

    }else
    {
        sleep(2);
        printf("Parent ID: %d, Child ID: %d\n",getpid(),child_process);
        printf("a = %d\n",a);
        wait(NULL);

    }

    return 0;
}