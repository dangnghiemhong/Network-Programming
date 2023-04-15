#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
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

    char buf[2048];
    int ret;

   
    FILE *fp = fopen(argv[2], "rb");
        if (fp == NULL)
        {
            perror("fopen() failed");
            exit(EXIT_FAILURE);
        }
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        rewind(fp);
        char *greeting = (char *)malloc(fsize + 1);
        if (greeting == NULL)
        {
            perror("malloc() failed");
            exit(EXIT_FAILURE);
        }
        memset(greeting, 0, fsize + 1);
        fread(greeting, fsize, 1, fp);
        fclose(fp);

        if (send(client, greeting, strlen(greeting), 0) == -1)
        {
            perror("send() failed");
            exit(EXIT_FAILURE);
        }
        free(greeting);

        while (1)
        {
            int bytes_received = recv(client, buf, sizeof(buf), 0);
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

            printf("Received %ld bytes from client: %s", strlen(buf), buf);

            fp = fopen(argv[3], "a");
            if (fp == NULL)
            {
                perror("fopen() failed");
                exit(EXIT_FAILURE);
            }
            fprintf(fp, "%s", buf);
            fclose(fp);
        }
    close(client);
    close(listener);
}