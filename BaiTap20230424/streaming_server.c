#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MAX_CLIENT 10

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Sai tham so");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server, MAX_CLIENT) == -1)
    {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    int client = accept(server, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client == -1)
    {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }
    printf("Connection from %s %d port [tcp/*] succeeded!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    int count = 0;
    char buf[20], receive[21];
    memset(buf, 'x', 9);
    memset(buf + 9, 0, 11);
    while (1)
    {
        memset(receive, 0, 21);
        int bytes_received = recv(client, receive, 20, 0);
        receive[bytes_received] = '\0';
        if (bytes_received == -1)
        {
            perror("recv() failed");
            exit(EXIT_FAILURE);
        }
        else if (bytes_received == 0)
        {
            printf("Count: %d\n", count);
            break;
        }
        strncat(buf, receive, 10);
        if (strstr(buf, "0123456789") != NULL)
        {
            count++;
        }
        strcpy(buf, buf + 10);
        strcat(buf, receive + 10);
        if (strstr(buf, "0123456789") != NULL)
        {
            count++;
        }
        strcpy(buf, buf + 10);
    }
    close(client);
    close(server);
    return 0;
}