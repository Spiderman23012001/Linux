#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define LISTEN_BACKLOG 10
#define BUFF_SIZE 256
#define handle_error(msg) \
    do{ perror(msg); exit(EXIT_FAILURE); }while(0);

void chat_func(int socket_fd)
{
    int numb_read, numb_write;
    char recevbuff[BUFF_SIZE];
    char sendbuff[BUFF_SIZE];

    // ghi 
    while(1)
    {
        // memset
        memset(&recevbuff,'0', BUFF_SIZE);
        memset(&sendbuff, '0', BUFF_SIZE);
        // ghi
        printf("Enter the message: ");
        fgets(sendbuff, BUFF_SIZE, stdin);

        numb_write = write(socket_fd, sendbuff, sizeof(sendbuff));
        if(numb_write == -1)
            handle_error("write error!\n");

        if(strncmp("exit", sendbuff, 4) == 0)
        {
            printf("Client exiting...\n");
            break;
        }
            
        // read
        numb_read = read(socket_fd, recevbuff, sizeof(recevbuff));
        if(numb_read == -1)
            handle_error("Read error!\n");
        if(strncmp("exit", recevbuff, 4) == 0)
        {
            printf("Server exiting ...\n");
            break;
        }
        printf("Message from Server: %s",recevbuff);
    }

    // read
}

int main(int argc, char * argv[])
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd == -1)
        handle_error("create socket client error!\n");

    if(argc < 3)
        handle_error("3 parameter. Command: ./client <ip address> <port number>");

    // coonect
    int port_number = atoi(argv[2]);
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port_number);
    if(inet_pton(AF_INET, argv[1], &client_addr.sin_addr) <= 0)
        handle_error("Error handle ip address\n");

    if(connect(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
        handle_error("connect failed!\n");

    chat_func(client_fd);
    close(client_fd);

    return 0;
}