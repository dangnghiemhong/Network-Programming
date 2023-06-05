#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buf[BUF_SIZE];

    // Khởi tạo socket chờ
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_sock, 5) < 0) {
        perror("listen");
        exit(1);
    }

    fd_set read_fds;
    int max_fd = server_sock;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);

        for (int i = 0; i < client_socks.size(); i++) {
            FD_SET(client_socks[i], &read_fds);
            if (client_socks[i] > max_fd) {
                max_fd = client_socks[i];
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(1);
        }

        if (FD_ISSET(server_sock, &read_fds)) {
            socklen_t client_len = sizeof(client_addr);
            client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
            if (client_sock < 0) {
                perror("accept");
                exit(1);
            }
            printf("New client connected\n");
            client_socks.push_back(client_sock);
        }

        for (int i = 0; i < client_socks.size(); i++) {
            if (FD_ISSET(client_socks[i], &read_fds)) {
                int len = recv(client_socks[i], buf, BUF_SIZE, 0);
                if (len < 0) {
                    perror("recv");
                    exit(1);
                } else if (len == 0) {
                    printf("Client disconnected\n");
                    close(client_socks[i]);
                    client_socks.erase(client_socks.begin() + i);
                } else {
                    buf[len] = '\0';
                    // Xử lý tin nhắn theo yêu cầu của đề bài
                    // ...
                    // Gửi kết quả trả về cho client
                    if (send(client_socks[i], buf, strlen(buf), 0) < 0) {
                        perror("send");
                        exit(1);
                    }
                }
            }
        }
    }

    close(server_sock);

    return 0;
}
