#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

pthread_t tid1;

static void* thread_handler(void *args)
{
    sleep(2);
    int *data = (int*) args;
    printf("Thread 1: a = %d\n", *data);
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    int a = 1;
    
    pthread_create(&tid1, NULL, thread_handler, &a); // Create a new process
    a = 5;
    pthread_join(tid1, NULL);

    return 0;
}