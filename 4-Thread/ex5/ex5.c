/*
Tạo một biến data chứa một số nguyên.
Tạo 5 threads đọc và 2 threads ghi:
Các thread đọc sẽ chỉ đọc data mà không thay đổi.
Các thread ghi sẽ tăng giá trị của data lên 1.
Sử dụng read-write lock để đảm bảo nhiều threads có thể đọc cùng lúc, nhưng chỉ một thread được ghi tại bất kỳ thời điểm nào.
In ra giá trị của data sau khi tất cả các threads hoàn thành.

*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_rwlock_t rwlock;
int data_shared = 0;

static void *read_handler(void *args)
{
    int id = *(int*) args;
    pthread_rwlock_rdlock(&rwlock);

    printf("Reader %d: data = %d\n", id, data_shared);
    for(int i = 0; i < 10; i++)
    {
        printf("Reading... %d %%\n", i*10);
        sleep(1);

    }

    pthread_rwlock_unlock(&rwlock);
    pthread_exit(NULL);

}

static void *write_handler(void *args)
{
    int id = *(int*) args;
    pthread_rwlock_wrlock(&rwlock);
    data_shared++;
    printf("Writer %d: data = %d\n", id, data_shared);
    for(int i = 0; i < 10; i++)
    {
        printf("Writing... %d %%\n", i*10);
        sleep(1);

    }
    pthread_rwlock_unlock(&rwlock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t read[5], write[3];
    int read_index[5] = {1,2,3,4,5};
    int write_index[3] = {1,2,3};

    pthread_rwlock_init(&rwlock, NULL);

    // taoj 2 thread writte
    for(int i = 0;i < 3; i++)
    {
        pthread_create(&write[i], NULL, write_handler, &write_index[i]);
    }
    
    //Taoj 5 thread read
    for(int i = 0;i < 5; i++)
    {
        pthread_create(&read[i], NULL, read_handler, &read_index[i]);
    }

    // thu hoi data read
    for(int i = 0; i < 5; i++)
    {
        pthread_join(read[i], NULL);
    }

    // thu hoi data write
    for(int i = 0; i < 3; i++)
    {
        pthread_join(write[i], NULL);
    }
    printf("Data: %d\n", data_shared);
    pthread_rwlock_destroy(&rwlock);
    return 0;
}