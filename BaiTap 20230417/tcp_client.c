#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
int main(int argc, char *argv[])
{
    // Kiểm tra đầu vào
    if (argc != 3)
    {
        printf("Error");
        return 1;
    }
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }
    printf("Connection to %s %s port [tcp/*] succeeded!\n", argv[1], argv[2]);
    char *msg = "Hello server\n";
    send(client, msg, strlen(msg), 0);

    char buf[1024];
    
    while (1)
    {
        printf("Enter a message: ");
        fgets(buf, sizeof(buf), stdin);

        if (send(client, buf, strlen(buf), 0) == -1)
        {
            perror("send() failed");
            return 1;
        }

        if (strcmp(buf, "exit\n") == 0)
        {
            break;
        }
    }
    close(client);
}