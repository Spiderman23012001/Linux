#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
// #include <sys/wait.h>
#include <pthread.h>

/*
************** Sử dụng mutex để đồng bộ dữ liệu vùng critical section ************************
************** Chỉ một luồng có thể truy cập share resource và được block bởi mutex***********
************** Nếu không dùng mutex sẽ xảy ra 3 thread chạy đồng thời --> race condition******
*/



#define MAX_COUNT 1000000

pthread_t tid1, tid2, tid3;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

int counter = 0;

static void *thread_handler(void *arg)
{

    pthread_mutex_lock(&lock1);
    for(int i = 0; i < MAX_COUNT; i++)
    {
        counter++;
    }
    pthread_mutex_unlock(&lock1);
    pthread_exit(NULL); // Exit the thread and return NULL to the calling thread.
}


int main(int argc, char *argv[])
{
    int id1, id2;
    printf("Before counter value: %d\n", counter);

    pthread_create(&tid1, NULL, thread_handler, NULL);
    pthread_create(&tid2, NULL, thread_handler, NULL);
    pthread_create(&tid3, NULL, thread_handler, NULL);
    
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
    
    printf("After counter value: %d\n", counter);
    printf("All thread completed\n");
    return 0;
}