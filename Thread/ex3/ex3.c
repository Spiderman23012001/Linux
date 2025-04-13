#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#define MAX_RD 10

int ready = 0;
int ramdom_number = 0;

pthread_t producer_thread, consumer_thread;

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t consumer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t producer_cond = PTHREAD_COND_INITIALIZER;


static void *producer_handler(void *args)
{
    for(int i = 0; i < MAX_RD; i++)
    {
        pthread_mutex_lock(&lock1);

        // wait signal from consumer
        while(ready == 1)
        {
            pthread_cond_wait(&producer_cond, &lock1);
        }

        ramdom_number = rand() % 100 + 1;
        printf("Produce generate: %d\n", ramdom_number);
        ready = 1;

        // send signal to consumer
        pthread_cond_signal(&consumer_cond);
    
        pthread_mutex_unlock(&lock1);
    }

    pthread_exit(NULL);


}

static void *consumer_handler(void *args)
{
    for(int i = 0; i < MAX_RD; i++)
    {
        pthread_mutex_lock(&lock1);
        // wait signal from produce
        while(ready==0)
        {
            pthread_cond_wait(&consumer_cond, &lock1);
        }

        printf("Consumer received: %d\n",ramdom_number);
        ready = 0;
        // send signal to produce
        pthread_cond_signal(&producer_cond);

        pthread_mutex_unlock(&lock1);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    pthread_create(&producer_thread, NULL, producer_handler, NULL);
    pthread_create(&consumer_thread, NULL, consumer_handler, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}