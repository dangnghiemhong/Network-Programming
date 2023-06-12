#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Sai tham so");
        return 1;
    }
    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[2]);
    addr.sin_port = htons(atoi(argv[3]));

    FILE *fp = fopen(argv[1], "rb");
    if (sendto(sender, argv[1], strlen(argv[1]), 0, (struct sockaddr *)&addr, sizeof(addr)) != strlen(argv[1]))
    {
        exit(EXIT_FAILURE);
    }

    char buf[1024];
    
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        if (sendto(sender, buf, strlen(buf), 0, (struct sockaddr *)&addr, sizeof(addr)) != strlen(buf))
        {
            perror("sendto() failed");
            exit(EXIT_FAILURE);
        }
        memset(buf, 0, sizeof(buf));
    }
    sendto(sender, NULL, 0, 0, (struct sockaddr *)&addr, sizeof(addr));

    printf("Sent file %s to %s:%d successfully!\n", argv[1], argv[2], atoi(argv[3]));

    fclose(fp);
    
    close(sender);

    return 0;
}
    
