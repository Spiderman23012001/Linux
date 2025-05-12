/*
Viết một chương trình tính tổng một mảng lớn gồm 1 triệu số nguyên.
 - Chia mảng thành 4 phần bằng nhau.
 - Tạo 4 threads, mỗi thread tính tổng một phần của mảng.
 - Sử dụng một biến tổng toàn cục và mutex để tổng hợp kết quả từ tất cả các threads.
 - In ra kết quả tổng của mảng sau khi các threads hoàn thành.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define MAX_THREAD 4

pthread_t tid[MAX_THREAD];
pthread_mutex_t lock_sum = PTHREAD_MUTEX_INITIALIZER;

long long sum_arr = 0;
int array[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};

typedef struct
{
    int start;
    int end;
}thread_data;

static void *sum_handler(void *args)
{
    thread_data *data = (thread_data*) args;

    pthread_mutex_lock(&lock_sum);
    for(int i = data->start; i < data->end; i++)
    {
        sum_arr += array[i];
    }
    pthread_mutex_unlock(&lock_sum);
    
    pthread_exit(NULL);
}




int main(int argc, char *argv[])
{
    thread_data thread_data[MAX_THREAD];
    int step = (sizeof(array)/sizeof(int)) / MAX_THREAD;

    for(int i = 0; i < MAX_THREAD; i++)
    {
        thread_data[i].start = i * step;
        thread_data[i].end = (i + 1) * step;
        pthread_create(&tid[i], NULL, sum_handler, &(thread_data[i]));
    }

    for(int i = 0; i < MAX_THREAD; i++)
    {
        pthread_join(tid[i], NULL);
    }

    printf("Sum of array: %lld\n",sum_arr);

    return 0;
}