/*
Tạo một mảng ngẫu nhiên chứa 100 số nguyên từ 1 đến 100.
Tạo hai threads: một thread đếm số chẵn và một thread đếm số lẻ trong mảng.
Khi hai threads hoàn tất việc đếm, in ra tổng số số chẵn và tổng số số lẻ.
Sử dụng pthread_join để đảm bảo rằng chương trình chỉ kết thúc khi cả hai threads hoàn thành nhiệm vụ.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int Arr[15] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};

pthread_t odd_thread, even_thread;

static void *odd_handler(void *args)
{
    int count_odd = 0, sum_odd = 0;
    for(int i = 0; i < sizeof(Arr)/sizeof(int); i++)
    {
        if(Arr[i] % 2 != 0)
        {
            sum_odd += Arr[i];
            count_odd++;
        }
    }
    printf("Number of odd: %d\n", count_odd);
    printf("Sum of odd: %d\n", sum_odd);
    pthread_exit(NULL);
}

static void *even_handler(void *args)
{
    int count_even = 0, sum_even = 0;
    for(int i = 0; i < sizeof(Arr)/sizeof(int); i++)
    {
        if(Arr[i] % 2 == 0)
        {
            sum_even += Arr[i];
            count_even++;
        }
    }
    printf("Number of even: %d\n", count_even);
    printf("Sum of even: %d\n", sum_even);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_create(&odd_thread, NULL, odd_handler, NULL);
    pthread_create(&even_thread, NULL, even_handler, NULL);

    pthread_join(odd_thread, NULL);
    pthread_join(even_thread, NULL);
    return 0;
}