#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sigtstp_handler(int signum) {
    printf("\nSIGTSTP ignored (handled with sigaction)\n");
    fflush(stdout);
}

int main() {
    struct sigaction sa;

    sa.sa_handler = sigtstp_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Cài đặt xử lý tín hiệu SIGTSTP
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Chạy chương trình. Nhấn Ctrl+Z để gửi SIGTSTP.\n");

    while (1) {
        printf("Running...\n");
    }

    return 0;
}

