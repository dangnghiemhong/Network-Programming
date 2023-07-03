#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 12345

typedef struct {
    int client_socket;
    pthread_t thread_id;
} Client;

Client clients[16];
pthread_mutex_t mutex;
int client_count = 0;

void *handle_client(void *arg) {
    Client *client = (Client *)arg;
    char buffer[MAX_BUFFER_SIZE];

    while (1) {
        // Nhận tin nhắn từ client
        int recv_bytes = recv(client->client_socket, buffer, sizeof(buffer), 0);
        if (recv_bytes <= 0) {
            // Client đã ngắt kết nối
            break;
        }

        // Chuyển tiếp tin nhắn cho client kia
        int other_client_index = (client == &clients[0]) ? 1 : 0;
        send(clients[other_client_index].client_socket, buffer, recv_bytes, 0);
    }

    // Đóng kết nối
    pthread_mutex_lock(&mutex);
    client_count--;

    if (client_count == 0) {
        // Nếu không còn client nào, đóng kết nối của cả hai
        close(clients[0].client_socket);
        close(clients[1].client_socket);
    }

    pthread_mutex_unlock(&mutex);

    // Hủy tiến trình của client
    pthread_cancel(client->thread_id);

    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Tạo socket server
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("Failed to create socket.\n");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    // Liên kết socket với địa chỉ và cổng
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to bind socket.\n");
        return 1;
    }

    // Lắng nghe kết nối từ client
    if (listen(server_socket, 2) < 0) {
        printf("Failed to listen on socket.\n");
        return 1;
    }

    printf("Chat server started.\n");

    pthread_mutex_init(&mutex, NULL);

    while (1) {
        // Chấp nhận kết nối từ client
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            printf("Failed to accept connection.\n");
            return 1;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_mutex_lock(&mutex);

        if (client_count < 2) {
            // Lưu trữ thông tin client vào hàng đợi
            clients[client_count].client_socket = client_socket;

            // Tạo một luồng để xử lý client
            if (pthread_create(&clients[client_count].thread_id, NULL, handle_client, &clients[client_count]) != 0) {
                printf("Failed to create thread for client.\n");
                pthread_mutex_unlock(&mutex);
                close(client_socket);
                continue;
            }

            client_count++;

            if (client_count % 2 = 0 && client_count > 1) {
                printf("Two clients connected. Chat session started.\n");
            }
        } //else {
        //     printf("Max number of clients reached. Connection rejected.\n");
        //     close(client_socket);
        // }

        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
