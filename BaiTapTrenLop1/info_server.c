#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX_LENGTH 1024

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Sai tham so.\n");
        return 1;
    }

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed\n");
        return 1;        
    }

    if (listen(listener, 5))
    {
        perror("listen() failed\n");
        return 1;
    }
    printf("Waiting for a new client ...\n");

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    int client = accept(listener, 
        (struct sockaddr *)&client_addr, 
        &client_addr_len);

    printf("Accepted socket %d from IP: %s:%d\n", client,inet_ntoa(client_addr.sin_addr),
        ntohs(client_addr.sin_port));

    char buf[1024];
    int ret;
    while (1)
        {
            int bytes_received = recv(client, buf, MAX_LENGTH, 0);
            if (bytes_received == -1)
            {
                perror("recv() failed");
                exit(EXIT_FAILURE);
            }
            buf[bytes_received] = '\0';

            if (strcmp(buf, "exit\n") == 0)
            {
                printf("Client from %s:%d disconnected\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                break;
            }

            while (1)
        {
            // Nhận dữ liệu từ client
            int bytes_received = recv(client, buf, MAX_LENGTH, 0);
            buf[bytes_received] = '\0';
            if (bytes_received == -1)
            {
                perror("recv() failed");
                return 1;
            }
            else if (bytes_received == 0 || strcmp(buf, "exit\n") == 0)
            {
                printf("Client from %s:%d disconnected\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                break;
            }

            // Phân tách dữ liệu thông tin máy tính
            char *token = strtok(buf, ";");
            printf("Hostname: %s\n", token);
            token = strtok(NULL, ";");
            printf("Number of disks: %s\n", token);
            int n = atoi(token);
            while (token = strtok(NULL, ";"))
            {
                printf("\t- Disk %s: ", token);
                token = strtok(NULL, ";");
                printf("%sGB\n", token);
            }
            printf("\n");
        }
            
        }

        close(client);
        close(listener);
    }


