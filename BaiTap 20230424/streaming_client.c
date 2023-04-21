#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MAX_LENGTH 1024

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Sai tham so");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("connect() failed");
        exit(EXIT_FAILURE);
    }
    printf("Connection to %s %s port [tcp/*] succeeded!\n", argv[1], argv[2]);

    FILE *fp = fopen(argv[3], "r");
    if (fp == NULL)
    {
        perror("Khong mo dc file");
        exit(EXIT_FAILURE);
    }

    char buf[21];
    memset(buf, 0, 21);
    while (!feof(fp))
    {

        fgets(buf, 21, fp);

        if (send(client, buf, strlen(buf), 0) == -1)
        {
            perror("send() failed");
            exit(EXIT_FAILURE);
        }
    }
    printf("Data sent successfully!\n");
    fclose(fp);
    close(client);
    return 0;
}