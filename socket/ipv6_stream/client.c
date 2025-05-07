#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFF_SIZE       256

#define handle_error(msg)\
    do{ perror(msg); exit(EXIT_FAILURE); }while(0);

int main(int argc, char *argv[])
{
    int port_no;
    int client_fd;
    struct sockaddr_in6 serv_addr;
    char sendbuff[BUFF_SIZE], recevbuff[BUFF_SIZE];
    int numb_read, numb_write;

    if (argc < 3) {
        printf("Usage: %s <IPv6 address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    } else {
        port_no = atoi(argv[2]);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    // 1. Create socket IPv6
    client_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (client_fd == -1)
        handle_error("socket()");

    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(port_no);

    // 2. Convert IPv6 address and set it
    if (inet_pton(AF_INET6, argv[1], &serv_addr.sin6_addr) <= 0)
        handle_error("inet_pton() error");

    // 3. Connect to the server
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("connect()");

    printf("Connected to server %s on port %d\n", argv[1], port_no);

    // 4. Chat loop
    while (1) {
        memset(sendbuff, 0, BUFF_SIZE);
        memset(recevbuff, 0, BUFF_SIZE);

        // Get message from user
        printf("Enter message to send: ");
        fgets(sendbuff, BUFF_SIZE, stdin);

        numb_write = write(client_fd, sendbuff, strlen(sendbuff));
        if (numb_write == -1)
            handle_error("write()");

        if (strncmp("exit", sendbuff, 4) == 0)
            break;

        numb_read = read(client_fd, recevbuff, sizeof(recevbuff));
        if (numb_read == -1)
            handle_error("read()");

        printf("Message from server: %s\n", recevbuff);

        if (strncmp("exit", recevbuff, 4) == 0)
            break;
    }

    close(client_fd);
    return 0;
}
