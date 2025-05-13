/*
 * chat.c - Peer-to-peer chat program using TCP sockets
 * Usage: ./chat <listening_port>
 *
 * Implements a command interface with commands:
 * help, myip, myport, connect, list, terminate, send, exit
 *
 * Handles multiple connections using select().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <fcntl.h>

#define MAX_CONNECTIONS 10
#define MAX_MESSAGE_LEN 100
#define MAX_COMMAND_LEN 256

typedef struct {
    int sockfd;
    struct sockaddr_in addr;
    int active;
} connection_t;

connection_t connections[MAX_CONNECTIONS];
int listen_port = 0;
int listen_sock = -1;

void print_help() {
    printf("Available commands:\n");
    printf("help - Display this help message\n");
    printf("myip - Display this process's IP address\n");
    printf("myport - Display the port this process is listening on\n");
    printf("connect <destination> <port no> - Connect to a peer\n");
    printf("list - List all connections\n");
    printf("terminate <connection id> - Terminate a connection\n");
    printf("send <connection id> <message> - Send a message to a connection\n");
    printf("exit - Close all connections and exit\n");
}

int get_my_ip(char *ip_str, size_t max_len) {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return -1;
    }
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL) {
        herror("gethostbyname");
        return -1;
    }
    for (int i = 0; he->h_addr_list[i] != NULL; i++) {
        struct in_addr *addr = (struct in_addr *)he->h_addr_list[i];
        // Skip loopback addresses
        if (strncmp(inet_ntoa(*addr), "127.", 4) != 0) {
            strncpy(ip_str, inet_ntoa(*addr), max_len);
            ip_str[max_len - 1] = '\0';
            return 0;
        }
    }
    return -1;
}

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

int connect_to_peer(const char *ip_str, int port) {
    if (port <= 0 || port > 65535) {
        printf("Error: Invalid port number\n");
        return -1;
    }
    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_str, &peer_addr.sin_addr) <= 0) {
        printf("Error: Invalid IP address\n");
        return -1;
    }
    if (is_self_connection(&peer_addr)) {
        printf("Error: Cannot connect to self\n");
        return -1;
    }
    if (is_duplicate_connection(&peer_addr)) {
        printf("Error: Duplicate connection\n");
        return -1;
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    if (connect(sockfd, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }
    int id = add_connection(sockfd, &peer_addr);
    if (id < 0) {
        printf("Error: Connection limit reached\n");
        close(sockfd);
        return -1;
    }
    printf("Connection established with %s:%d\n", ip_str, port);
    return id;
}

void send_message(int id, const char *message) {
    if (id <= 0 || id > MAX_CONNECTIONS || !connections[id - 1].active) {
        printf("Error: Invalid connection id %d\n", id);
        return;
    }
    if (strlen(message) > MAX_MESSAGE_LEN) {
        printf("Error: Message too long (max %d characters)\n", MAX_MESSAGE_LEN);
        return;
    }
    int sockfd = connections[id - 1].sockfd;
    ssize_t sent = send(sockfd, message, strlen(message), 0);
    if (sent < 0) {
        perror("send");
    } else {
        printf("Message sent to %d\n", id);
    }
}

void close_all_connections() {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].active) {
            close(connections[i].sockfd);
            connections[i].active = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <listening_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    listen_port = atoi(argv[1]);
    if (listen_port <= 0 || listen_port > 65535) {
        fprintf(stderr, "Invalid port number\n");
        exit(EXIT_FAILURE);
    }

    // Initialize connections array
    memset(connections, 0, sizeof(connections));

    // Create listening socket
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons(listen_port);

    if (bind(listen_sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
        perror("bind");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_sock, MAX_CONNECTIONS) < 0) {
        perror("listen");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\n", listen_port);

    fd_set master_set, read_fds;
    int fdmax = listen_sock;

    FD_ZERO(&master_set);
    FD_SET(listen_sock, &master_set);
    FD_SET(STDIN_FILENO, &master_set);
    if (STDIN_FILENO > fdmax) fdmax = STDIN_FILENO;

    char input_buf[MAX_COMMAND_LEN];

    while (1) {
        read_fds = master_set;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // Check for new incoming connection
        if (FD_ISSET(listen_sock, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);
            int newfd = accept(listen_sock, (struct sockaddr *)&client_addr, &addrlen);
            if (newfd < 0) {
                perror("accept");
            } else {
                if (is_self_connection(&client_addr)) {
                    printf("Rejected self-connection attempt\n");
                    close(newfd);
                } else if (is_duplicate_connection(&client_addr)) {
                    printf("Rejected duplicate connection attempt\n");
                    close(newfd);
                } else {
                    int id = add_connection(newfd, &client_addr);
                    if (id < 0) {
                        printf("Connection limit reached, rejecting connection\n");
                        close(newfd);
                    } else {
                        printf("Connection accepted from %s:%d\n",
                               inet_ntoa(client_addr.sin_addr),
                               ntohs(client_addr.sin_port));
                        FD_SET(newfd, &master_set);
                        if (newfd > fdmax) fdmax = newfd;
                    }
                }
            }
        }

        // Check for input from stdin
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
                printf("Error reading input\n");
                continue;
            }
            // Remove trailing newline
            input_buf[strcspn(input_buf, "\n")] = 0;

            // Parse command
            char *cmd = strtok(input_buf, " ");
            if (cmd == NULL) continue;

            if (strcmp(cmd, "help") == 0) {
                print_help();
            } else if (strcmp(cmd, "myip") == 0) {
                char ip[INET_ADDRSTRLEN];
                if (get_my_ip(ip, sizeof(ip)) == 0) {
                    printf("IP Address: %s\n", ip);
                } else {
                    printf("Unable to get IP address\n");
                }
            } else if (strcmp(cmd, "myport") == 0) {
                printf("Listening on port: %d\n", listen_port);
            } else if (strcmp(cmd, "connect") == 0) {
                char *ip_str = strtok(NULL, " ");
                char *port_str = strtok(NULL, " ");
                if (ip_str == NULL || port_str == NULL) {
                    printf("Usage: connect <destination> <port no>\n");
                } else {
                    int port = atoi(port_str);
                    connect_to_peer(ip_str, port);
                }
            } else if (strcmp(cmd, "list") == 0) {
                list_connections();
            } else if (strcmp(cmd, "terminate") == 0) {
                char *id_str = strtok(NULL, " ");
                if (id_str == NULL) {
                    printf("Usage: terminate <connection id>\n");
                } else {
                    int id = atoi(id_str);
                    if (id > 0) {
                        if (id <= MAX_CONNECTIONS && connections[id - 1].active) {
                            FD_CLR(connections[id - 1].sockfd, &master_set);
                            remove_connection(id);
                        } else {
                            printf("Error: Invalid connection id %d\n", id);
                        }
                    }
                }
            } else if (strcmp(cmd, "send") == 0) {
                char *id_str = strtok(NULL, " ");
                char *msg = strtok(NULL, "");
                if (id_str == NULL || msg == NULL) {
                    printf("Usage: send <connection id> <message>\n");
                } else {
                    int id = atoi(id_str);
                    if (id > 0) {
                        send_message(id, msg);
                    }
                }
            } else if (strcmp(cmd, "exit") == 0) {
                printf("Exiting...\n");
                close_all_connections();
                close(listen_sock);
                exit(EXIT_SUCCESS);
            } else {
                printf("Unknown command. Type 'help' for a list of commands.\n");
            }
        }

        // Check for data from connected peers
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (connections[i].active && FD_ISSET(connections[i].sockfd, &read_fds)) {
                char buf[MAX_MESSAGE_LEN + 1];
                ssize_t nbytes = recv(connections[i].sockfd, buf, MAX_MESSAGE_LEN, 0);
                if (nbytes <= 0) {
                    if (nbytes == 0) {
                        printf("Connection %d closed by peer\n", i + 1);
                    } else {
                        perror("recv");
                    }
                    close(connections[i].sockfd);
                    FD_CLR(connections[i].sockfd, &master_set);
                    connections[i].active = 0;
                } else {
                    buf[nbytes] = '\0';
                    printf("Message received from %s\n", inet_ntoa(connections[i].addr.sin_addr));
                    printf("Sender's Port: %d\n", ntohs(connections[i].addr.sin_port));
                    printf("Message: \"%s\"\n", buf);
                }
            }
        }
    }

    return 0;
}
