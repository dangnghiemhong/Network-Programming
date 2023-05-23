#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
   
    int sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in sender_addr;
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_addr.s_addr = inet_addr(argv[1]);
    sender_addr.sin_port = htons(atoi(argv[2]));

    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in r_addr;
    r_addr.sin_family = AF_INET;
    r_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    r_addr.sin_port = htons(atoi(argv[3]));

    bind(receiver, (struct sockaddr *)&client_addr, sizeof(client_addr));

    fd_set fdread, fdtest;
    FD_ZERO(&fdread);
    FD_SET(0, &fdread);
    FD_SET(receiver, &fdread);

    char buf[256];

    while (1)
    {
        fdtest = fdread;
        int ret = select(receiver + 1, &fdtest, NULL, NULL, NULL);
        if (ret < 0)
        {
            perror("select() failed");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &fdtest))
        {
            fgets(buf, sizeof(buf), stdin);
            sendto(sender, buf, strlen(buf), 0, 
                (struct sockaddr *)&saddr, sizeof(saddr));
        }

        if (FD_ISSET(receiver, &fdtest))
        {
            ret = recvfrom(receiver, buf, sizeof(buf), 0, NULL, NULL);
            buf[ret] = 0;
            printf("Received: %s\n", buf);
        }
    }

    close(sender);
    close(receiver);
}