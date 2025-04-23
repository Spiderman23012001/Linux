#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int counter_seconds = 0;

void signal_handler(int num)
{
    printf("Timer: %d seconds\n",counter_seconds);
    counter_seconds++;
    if(counter_seconds == 10)
    {
        exit(EXIT_SUCCESS);
    }
    // alarm(1);
}

int main(int argc, char *argv[])
{   
    signal(SIGALRM, signal_handler);
    while(1)
    {
        alarm(1);
        pause();
    }
    
    return 0;
}