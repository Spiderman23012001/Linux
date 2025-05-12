#include <stdio.h>
#include <signal.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Flag to indicate SIGTERM received
volatile sig_atomic_t term_received = 0;

// Signal handler for SIGINT
void sigint_handler(int sig) {
    printf("SIGINT received.\n");
}

// Signal handler for SIGTERM
void sigterm_handler(int sig) {
    term_received = 1;
}

int main() {
    struct sigaction sa_int, sa_term;
    struct pollfd fds[1];
    char buffer[1024];

    // Block SIGINT and SIGTERM to prevent interruption during poll()
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) == -1) {
        perror("Failed to block signals");
        return 1;
    }

    // Set up SIGINT handler
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("Failed to set SIGINT handler");
        return 1;
    }

    // Set up SIGTERM handler
    sa_term.sa_handler = sigterm_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("Failed to set SIGTERM handler");
        return 1;
    }

    // Set up poll() to monitor stdin
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    printf("Program is running. Enter text (or Ctrl+C for SIGINT, or send SIGTERM to exit).\n");

    while (!term_received) {
        // Check for pending signals
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGINT)) {
            sigint_handler(SIGINT);
            // Clear SIGINT by unblocking and re-blocking
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            sigprocmask(SIG_BLOCK, &mask, NULL);
        }
        if (sigismember(&pending, SIGTERM)) {
            sigterm_handler(SIGTERM);
            break;
        }

        // Poll for input with timeout
        int ret = poll(fds, 1, 1000); // 1-second timeout
        if (ret == -1) {
            if (errno == EINTR) continue; // Interrupted by signal, retry
            perror("Poll error");
            return 1;
        }

        // Check if input is available
        if (ret > 0 && (fds[0].revents & POLLIN)) {
            ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("Input received: %s", buffer);
            }
        }
    }

    printf("Received SIGTERM, exiting.\n");
    return 0;
}
