#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include "connection.h"
#include "network.h"
#include "message.h"

int listen_port = 0;
int listen_sock = -1;

void print_help() {
    printf("-----------------------------------------------------------------------------------\n");
    printf("| Available Commands                                                              |\n");
    printf("-----------------------------------------------------------------------------------\n");
    printf("| Command                         | Description                                   |\n");
    printf("-----------------------------------------------------------------------------------\n");
    printf("| help                            | Display this help message                     |\n");
    printf("| myip                            | Display this process's IP address             |\n");
    printf("| myport                          | Display the port this process is listening on |\n");
    printf("| connect <destination> <port no> | Connect to a peer                             |\n");
    printf("| list                            | List all connections                          |\n");
    printf("| terminate <connection id>       | Terminate a connection                        |\n");
    printf("| send <connection id> <message>  | Send a message to a connection                |\n");
    printf("| exit                            | Close all connections and exit                |\n");
    printf("-----------------------------------------------------------------------------------\n");
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
    // This is done in connection.c static array

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

    char input_buf[256];

    while (1) {
        printf("Enter the command: ");
        fflush(stdout);
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
                    if (port <= 0 || port > 65535) {
                        printf("Error: Invalid port number\n");
                    } else {
                        // Connect to peer
                        // Create socket and connect
                        struct sockaddr_in peer_addr;
                        memset(&peer_addr, 0, sizeof(peer_addr));
                        peer_addr.sin_family = AF_INET;
                        peer_addr.sin_port = htons(port);
                        if (inet_pton(AF_INET, ip_str, &peer_addr.sin_addr) <= 0) {
                            printf("Error: Invalid IP address\n");
                        } else if (is_self_connection(&peer_addr)) {
                            printf("Error: Cannot connect to self\n");
                        } else if (is_duplicate_connection(&peer_addr)) {
                            printf("Error: Duplicate connection\n");
                        } else {
                            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
                            if (sockfd < 0) {
                                perror("socket");
                            } else if (connect(sockfd, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) < 0) {
                                perror("connect");
                                close(sockfd);
                            } else {
                                int id = add_connection(sockfd, &peer_addr);
                                if (id < 0) {
                                    printf("Error: Connection limit reached\n");
                                    close(sockfd);
                                } else {
                                    printf("Connection established with %s:%d\n", ip_str, port);
                                    FD_SET(sockfd, &master_set);
                                    if (sockfd > fdmax) fdmax = sockfd;
                                }
                            }
                        }
                    }
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
                        if (id <= MAX_CONNECTIONS && is_connection_active(id)) {
                            int sockfd = get_connection_sockfd(id);
                            FD_CLR(sockfd, &master_set);
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
        for (int i = 1; i <= MAX_CONNECTIONS; i++) {
            if (is_connection_active(i)) {
                int sockfd = get_connection_sockfd(i);
                if (FD_ISSET(sockfd, &read_fds)) {
                    char buf[101];
                    ssize_t nbytes = recv(sockfd, buf, 100, 0);
                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            printf("Connection %d closed by peer\n", i);
                        } else {
                            perror("recv");
                        }
                        close(sockfd);
                        FD_CLR(sockfd, &master_set);
                        remove_connection(i);
                    } else {
                        buf[nbytes] = '\0';
                        // We do not have direct access to connections array here, so we get peer info from socket
                        struct sockaddr_in peer_addr;
                        socklen_t addr_len = sizeof(peer_addr);
                        if (getpeername(sockfd, (struct sockaddr *)&peer_addr, &addr_len) == 0) {
                            printf("Message received from %s\n", inet_ntoa(peer_addr.sin_addr));
                            printf("Sender's Port: %d\n", ntohs(peer_addr.sin_port));
                        } else {
                            printf("Message received from unknown peer\n");
                        }
                        printf("Message: \"%s\"\n", buf);
                    }
                }
            }
        }
    }

    return 0;
}
