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
        char mssv[MAX_LENGTH], hoten[MAX_LENGTH], ngaysinh[MAX_LENGTH], diem[MAX_LENGTH];
        memset(mssv, 0, MAX_LENGTH);
        memset(hoten, 0, MAX_LENGTH);
        memset(ngaysinh, 0, MAX_LENGTH);
        memset(diem, 0, MAX_LENGTH);

        printf("Enter student information:\n");
        printf("- MSSV: ");
        fgets(mssv, MAX_LENGTH, stdin);
        mssv[strcspn(mssv, "\n")] = 0;

        printf("- Họ tên: ");
        fgets(hoten, MAX_LENGTH, stdin);
        hoten[strcspn(hoten, "\n")] = 0;

        printf("- Ngày sinh: ");
        fgets(ngaysinh, MAX_LENGTH, stdin);
        ngaysinh[strcspn(ngaysinh, "\n")] = 0;

        printf("- Điểm trung bình: ");
        fgets(diem, MAX_LENGTH, stdin);
        diem[strcspn(diem, "\n")] = 0;

        char buffer[4 * MAX_LENGTH + 1];
        memset(buffer, 0, 4 * MAX_LENGTH + 1);
        sprintf(buffer, "%s %s %s %s\n", mssv, hoten, ngaysinh, diem);

        if (send(client, buffer, strlen(buffer), 0) == -1)
        {
            perror("send() failed");
            return 1;
        }
        printf("Data sent successfully\n\n");

        char choice[MAX_LENGTH];
        memset(choice, 0, MAX_LENGTH);
        printf("Do you want to continue? (y/n): ");
        fgets(choice, MAX_LENGTH, stdin);
        choice[strcspn(choice, "\n")] = 0;
        if (strcmp(choice, "n") == 0)
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