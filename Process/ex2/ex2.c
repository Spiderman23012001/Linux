#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    pid_t pid = fork(); // Create a new process
    char *env_var = getenv("MYCMD");

    if (env_var == NULL) {
        fprintf(stderr, "Biến môi trường MYCMD chưa được thiết lập!\n");
        return 1;
    }
    if(pid < 0)
    {
        // Fork failed
        perror("Fork failed");
        return 1;

    }else if(pid == 0)
    {
        // Child process
        if(env_var == NULL || strcmp(env_var,"1") == 0)
        {
            printf("RUNNING LS\n");
            execlp("ls","ls",NULL);
        }else if(strcmp(env_var,"2") == 0)
        {
            printf("RUNNING DATE\n");
            execlp("date","date",NULL);
        }else
        {
            printf("Invalid argument\n");
            return -1;
        }        
    }else
    {
        //parent process
        printf("Parent process PID: %d, Child process PID: %d\n\n", getpid(), pid);
        wait(NULL);
        printf("Child process completed\n");
    }
    return 0;
}