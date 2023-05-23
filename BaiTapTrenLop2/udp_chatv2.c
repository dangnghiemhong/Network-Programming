#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <recv_port>\n", argv[0]);
        exit(1);
    }

    int recv_sock, send_sock;
    struct sockaddr_in recv_addr, send_addr;
    char buf[BUF_SIZE];

    // Khởi tạo socket nhận
    recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (recv_sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    recv_addr.sin_port = htons(atoi(argv[3]));

    if (bind(recv_sock, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // Khởi tạo socket gửi
    send_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (send_sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&send_addr, 0, sizeof(send_addr));
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = inet_addr(argv[1]);
    send_addr.sin_port = htons(atoi(argv[2]));

    fd_set read_fds;
    printf("Connected. Start chatting:\n");
    int max_fd = (recv_sock > send_sock) ? recv_sock : send_sock;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(recv_sock, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(recv_sock, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int len = recvfrom(recv_sock, buf, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
            if (len < 0) {
                perror("recvfrom");
                exit(1);
            }
            buf[len] = '\0';
            printf("Received message: %s\n", buf);
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(buf, BUF_SIZE, stdin);
            int len = strlen(buf);
            if (sendto(send_sock, buf, len, 0, (struct sockaddr *)&send_addr, sizeof(send_addr)) < 0) {
                perror("sendto");
                exit(1);
            }
        }
    }

    close(recv_sock);
    close(send_sock);

    return 0;
}
