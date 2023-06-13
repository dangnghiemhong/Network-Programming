#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 8080

void handle_client(int client_socket) {
    char file_list[MAX_BUFFER_SIZE];

    // Mở thư mục và lấy danh sách các file
    FILE *dir = popen("~/Network-Programming/BaiTapTrenLop3", "r");
    if (dir == NULL) {
        printf("Failed to open directory.\n");
        return;
    }

    // Đọc danh sách các file vào buffer
    size_t n = fread(file_list, sizeof(char), sizeof(file_list), dir);
    pclose(dir);

    if (n > 0) {
        // Gửi danh sách các file cho client
        char response[MAX_BUFFER_SIZE];
        sprintf(response, "OK%d\r\n%s\r\n\r\n", n, file_list);
        send(client_socket, response, strlen(response), 0);

        // Nhận tên file từ client
        char file_name[MAX_BUFFER_SIZE];
        recv(client_socket, file_name, sizeof(file_name), 0);

        // Kiểm tra nếu file tồn tại
        if (strstr(file_list, file_name) != NULL) {
            // Gửi phản hồi OK và kích thước file
            char file_path[256];
            sprintf(file_path, "~/Network-Programming/BaiTapTrenLop3/%s", file_name);

            FILE *file = fopen(file_path, "rb");
            if (file == NULL) {
                printf("Failed to open file.\n");
                return;
            }

            fseek(file, 0, SEEK_END);
            int file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            sprintf(response, "OK %d\r\n", file_size);
            send(client_socket, response, strlen(response), 0);

            // Gửi nội dung file cho client
            char buffer[MAX_BUFFER_SIZE];
            size_t read_bytes;
            while ((read_bytes = fread(buffer, sizeof(char), sizeof(buffer), file)) > 0) {
                send(client_socket, buffer, read_bytes, 0);
            }

            fclose(file);
        } else {
            // Gửi thông báo lỗi và yêu cầu client gửi lại tên file
            char response[] = "ERROR FileNotFound\r\n";
            send(client_socket, response, strlen(response), 0);
        }
    } else {
        // Gửi thông báo lỗi nếu không có file nào trong thư mục
        char response[] = "ERROR Nofilestodownload\r\n";
        send(client_socket, response, strlen(response), 0);
    }

    // Đóng kết nối
    close(client_socket);
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
    if (listen(server_socket, 5) < 0) {
        printf("Failed to listen on socket.\n");
        return 1;
    }

    printf("File server started.\n");

    while (1) {
        // Chấp nhận kết nối từ client
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            printf("Failed to accept connection.\n");
            return 1;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Tạo một tiến trình con để xử lý client kết nối
        pid_t pid = fork();

        if (pid == 0) {
            // Tiến trình con
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        } else if (pid > 0) {
            // Tiến trình cha
            close(client_socket);
        } else {
            printf("Failed to create child process.\n");
            return 1;
        }
    }

    // Đóng socket server
    close(server_socket);

    return 0;
}
