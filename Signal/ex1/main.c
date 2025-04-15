/*
Sử dụng hàm signal() để đăng ký một hàm xử lý cho tín hiệu SIGINT.
Trong hàm xử lý, in ra thông báo "SIGINT received" mỗi khi nhận được tín hiệu SIGINT.
Chương trình sẽ tiếp tục chạy cho đến khi nhận được SIGINT lần thứ 3, sau đó kết thúc.

*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int count = 0;

void sigal_handler1(int num)
{
    count++;
    printf("SIGINT received\n");
    if(count == 3)
    {
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigal_handler1);
    printf("process ID: %d\n", getpid());
    while(1);
    return 0;
}