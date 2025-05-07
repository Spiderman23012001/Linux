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
        memset(sendbuff, '0', BUFF_SIZE);
        memset(recevbuff, '0', BUFF_SIZE);

        // Read data from socket
        numb_read = read(new_socket_fd, recevbuff, sizeof(recevbuff));
        if(numb_read == -1)
            printf(" Read data error!\n");

        if(strncmp("exit", recevbuff, 4) == 0)
        {
            system("clear");
            break;
        }

        printf("Message from client: %s\n", recevbuff);
        /* Get input from keyboard*/
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
    int port_no, opt;
    int server_fd, new_socket_fd;
    struct sockaddr_in serv_addr, client_addr;

    // Read port number
    if(argc < 2)
    {
        printf("No port provied. Command: ./server <port number>\n");
        exit(EXIT_FAILURE);
    }else
    {
        port_no = atoi(argv[1]);
    }

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));

    // 1. Create socket()
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1)
    {
        handle_error("Socket()\n");
    }
    // Ngan loi: "Already in use"
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        handle_error("setsockopt!\n")

    // 2. bind()
    //Initialzation Address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);
    if(inet_pton(AF_INET, "100.66.95.128", &serv_addr.sin_addr) <= 0)
        handle_error("inet pton error for ip address. ");

    if(bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
        handle_error("binding error!\n");
    // 3. listen()
    if(listen(server_fd, LISTEN_BACKLOG) == -1)
        handle_error("Listening error!\n");

    
    socklen_t client_len = sizeof(client_addr);

    while(1)
    {
        printf("Server is listening at port: %d \n ... \n", port_no);
        // 4. accept()
        new_socket_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len );
        if(new_socket_fd == -1)
        {
            handle_error("Accept error!\n");
            system("clear");
        }

        printf("Server: Connected!\n");
        chat_func(new_socket_fd);
    }
    // 6. close()
    close(server_fd);

    return 0;
}