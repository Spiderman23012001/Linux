#ifndef CONNECTION_H
#define CONNECTION_H

#include <netinet/in.h>

#define MAX_CONNECTIONS 10

typedef struct {
    int sockfd;
    struct sockaddr_in addr;
    int active;
} connection_t;

int add_connection(int sockfd, struct sockaddr_in *addr);
void remove_connection(int id);
void list_connections();
int is_duplicate_connection(struct sockaddr_in *addr);
int is_self_connection(struct sockaddr_in *addr);
void close_all_connections();
int get_connection_sockfd(int id);
int is_connection_active(int id);

#endif // CONNECTION_H
