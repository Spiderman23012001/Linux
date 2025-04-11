#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


/*
*******************Sử dụng WIFEXITED lấy trạng thái kết thúc của child process********************
*******************Sử dụng WEXITSTATUS để lấy mã thoát của child process**************************
*/

int main(int argc, char *argv[])
{
    pid_t pid = fork(); // Create a new process
    int status;

    if(pid < 0)
    {
        // Fork failed
        perror("Fork failed");
        return 1;

    }else if(pid == 0)
    {
        // Child process
        printf("Child process iD %d",getpid());
        exit(42)
    
    }else
    {
        //parent process
        printf("Parent process PID: %d, Child process PID: %d\n\n", getpid(), pid);
        wait(&status);
        if(WIFEXITED(status))
        {
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        }else 
        {
            printf("Child process terminated abnormally\n");
        }
    }
    return 0;
}