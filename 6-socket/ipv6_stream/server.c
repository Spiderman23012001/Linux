#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define LISTEN_BACKLOG  10
#define BUFF_SIZE       256

#define handle_error(msg)\
    do{ perror(msg); exit(EXIT_FAILURE); }while(0);

// chat function
void chat_func(int new_socket_fd)
{
    int numb_read, numb_write;
    char sendbuff[BUFF_SIZE];
    char recevbuff[BUFF_SIZE];

    while(1)
    {
        memset(sendbuff, 0, BUFF_SIZE);
        memset(recevbuff, 0, BUFF_SIZE);

        numb_read = read(new_socket_fd, recevbuff, sizeof(recevbuff));
        if(numb_read == -1)
            printf(" Read data error!\n");

        if(strncmp("exit", recevbuff, 4) == 0)
        {
            system("clear");
            break;
        }

        printf("Message from client: %s\n", recevbuff);
        printf("Please response: ");
        fgets(sendbuff, BUFF_SIZE, stdin);

        numb_write = write(new_socket_fd, sendbuff, sizeof(sendbuff)); 
        if(numb_write == -1)
            handle_error("Sending error!\n");

        if(strncmp("exit", sendbuff, 4) == 0)
        {
            system("clear");
            break;
        }

        sleep(1);
    }
    close(new_socket_fd);
}

int main(int argc, char * argv[])
{
    int port_no, opt = 1;
    int server_fd, new_socket_fd;
    struct sockaddr_in6 serv_addr, client_addr;

    if(argc < 2)
    {
        printf("No port provided. Command: ./server <port number>\n");
        exit(EXIT_FAILURE);
    } else {
        port_no = atoi(argv[1]);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // 1. Create socket IPv6
    server_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if(server_fd == -1)
        handle_error("socket()");

    // display-addressbind bind
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        handle_error("setsockopt");

    // 2. bind()
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(port_no);
    // serv_addr.sin6_addr = in6addr_any;

    // if(inet_pton(AF_INET6, "::1", &serv_addr.sin6_addr) <= 0)
    //     handle_error("inet_pton IPv6 error");


    // Bindinj
    if(inet_pton(AF_INET6, "fd7a:115c:a1e0::3201:5932", &serv_addr.sin6_addr) <= 0)  // "::1" = localhost IPv6
        handle_error("inet_pton IPv6 error");

    if(bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
        handle_error("bind");

    // 3. listen()
    if(listen(server_fd, LISTEN_BACKLOG) == -1)
        handle_error("listen");

    socklen_t client_len = sizeof(client_addr);

    while(1)
    {
        printf("Server is listening on port: %d\n", port_no);
        new_socket_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if(new_socket_fd == -1)
        {
            perror("accept error");
            continue;
        }

        char client_ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &client_addr.sin6_addr, client_ip, sizeof(client_ip));
        printf("Server: Connected to %s\n", client_ip);

        chat_func(new_socket_fd);
    }

    close(server_fd);
    return 0;
}
