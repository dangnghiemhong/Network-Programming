#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>

#define MAX_LENGTH 1024
int main(int argc, char *argv[])
{
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

    while (1)
    {
        char nameComputer[MAX_LENGTH] ,nameDisk[MAX_LENGTH][MAX_LENGTH];
        int sizeDisk[MAX_LENGTH];
        memset(nameComputer, 0, MAX_LENGTH);
        memset(nameDisk, 0, MAX_LENGTH);

        printf("Enter computer information:\n");
        printf("- Name: ");
        fgets(nameComputer, MAX_LENGTH, stdin);
        nameComputer[strcspn(nameComputer, "\n")] = 0;

        int numberDisk;
        printf("- So o dia: ");
        scanf("%d", &numberDisk);
        getchar();

        for (int i = 0; i < numberDisk; i++)
        {
            memset(nameDisk[i], 0, MAX_LENGTH);
            printf("\t- Enter nameComputer disk %d: ", i + 1);
            fgets(nameDisk[i], MAX_LENGTH, stdin);
            nameDisk[i][strcspn(nameDisk[i], "\n")] = 0;

            printf("\t- Enter size disk %d: ", i + 1);
            scanf("%d", &sizeDisk[i]);
            getchar();
        }

        char buffer[MAX_LENGTH];
        memset(buffer, 0, MAX_LENGTH);
        sprintf(buffer, "%s;%d", nameComputer, numberDisk);
        for (int i = 0; i < numberDisk; i++)
        {
            sprintf(buffer, "%s;%s;%d", buffer, nameDisk[i], sizeDisk[i]);
        }


        if (send(client, buffer, strlen(buffer), 0) == -1)
        {
            perror("send() failed");
            return 1;
        }
        printf("Data sent successfully\n\n");

        char bfer[MAX_LENGTH];
        memset(bfer, 0, MAX_LENGTH);
        printf("Do you want to continue? (y/n): ");
        fgets(bfer, MAX_LENGTH, stdin);
        bfer[strcspn(bfer, "\n")] = 0;
        if (strcmp(bfer, "n") == 0)
        {
            if(send(client, "exit\n", 5, 0) == -1)
            {
                perror("send() failed");
                return 1;
            }
            break;
        }
    }

    close(client);
    return 0;
}