#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
// #include <sys/wait.h>
#include <pthread.h>

pthread_t tid1, tid2;
static void *thread_handler(void *arg)
{
    if(pthread_equal(tid1, pthread_self()))
    {
        printf("Thread 1 is running \n");
    }
    else if(pthread_equal(tid2, pthread_self()))
    {
        printf("Thread 2 is running \n");
    }
    else
    {
        printf("Unknown thread\n");
    }

    pthread_exit(NULL); // Exit the thread and return NULL to the calling thread.
}


int main(int argc, char *argv[])
{
    int id1, id2;
    pthread_create(&tid1, NULL, thread_handler, &id1);
    pthread_create(&tid2, NULL, thread_handler, &id2);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    printf("All thread completed\n");
    return 0;
}