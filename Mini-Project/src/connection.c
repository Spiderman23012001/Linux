#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "connection.h"
#include "network.h"

extern int listen_port;

static connection_t connections[MAX_CONNECTIONS];

int add_connection(int sockfd, struct sockaddr_in *addr) {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (!connections[i].active) {
            connections[i].sockfd = sockfd;
            connections[i].addr = *addr;
            connections[i].active = 1;
            return i + 1; // connection id is index+1
        }
    }
    return -1; // no space
}

void remove_connection(int id) {
    if (id <= 0 || id > MAX_CONNECTIONS || !connections[id - 1].active) {
        printf("Error: Invalid connection id %d\n", id);
        return;
    }
    close(connections[id - 1].sockfd);
    connections[id - 1].active = 0;
    printf("Connection %d terminated\n", id);
}

void list_connections() {
    printf("id: IP address\tPort No.\n");
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].active) {
            printf("%d: %s\t%d\n", i + 1,
                   inet_ntoa(connections[i].addr.sin_addr),
                   ntohs(connections[i].addr.sin_port));
        }
    }
}

int is_duplicate_connection(struct sockaddr_in *addr) {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].active) {
            if (connections[i].addr.sin_addr.s_addr == addr->sin_addr.s_addr &&
                connections[i].addr.sin_port == addr->sin_port) {
                return 1;
            }
        }
    }
    return 0;
}

int is_self_connection(struct sockaddr_in *addr) {
    char my_ip[INET_ADDRSTRLEN];
    if (get_my_ip(my_ip, sizeof(my_ip)) != 0) {
        return 0;
    }
    char peer_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr->sin_addr), peer_ip, sizeof(peer_ip));
    if (strcmp(my_ip, peer_ip) == 0 && ntohs(addr->sin_port) == listen_port) {
        return 1;
    }
    return 0;
}

void close_all_connections() {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].active) {
            close(connections[i].sockfd);
            connections[i].active = 0;
        }
    }
}

int get_connection_sockfd(int id) {
    if (id <= 0 || id > MAX_CONNECTIONS || !connections[id - 1].active) {
        return -1;
    }
    return connections[id - 1].sockfd;
}

int is_connection_active(int id) {
    if (id <= 0 || id > MAX_CONNECTIONS) {
        return 0;
    }
    return connections[id - 1].active;
}
