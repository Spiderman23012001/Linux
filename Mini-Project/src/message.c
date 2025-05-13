#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "message.h"
#include "connection.h"

#define MAX_MESSAGE_LEN 100

void send_message(int id, const char *message) {
    if (id <= 0 || id > MAX_CONNECTIONS || !is_connection_active(id)) {
        printf("Error: Invalid connection id %d\n", id);
        return;
    }
    if (strlen(message) > MAX_MESSAGE_LEN) {
        printf("Error: Message too long (max %d characters)\n", MAX_MESSAGE_LEN);
        return;
    }
    int sockfd = get_connection_sockfd(id);
    ssize_t sent = send(sockfd, message, strlen(message), 0);
    if (sent < 0) {
        perror("send");
    } else {
        printf("Message sent to %d\n", id);
    }
}
