#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./my_socket"
#define BUFFER_SIZE 256

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
	
    unlink(SOCKET_PATH);
    // �������� ������
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // �������� ������� ������, ���� �� ����������
    unlink(SOCKET_PATH);

    // ��������� ������ ������
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // �������� ������ � ������
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // �������� ����������� �������
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("������ �������. �������� ����������� �������...\n");

    // �������� �����������
    if ((client_fd = accept(server_fd, NULL, NULL)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // ������ ������ �� �������
    while (1) {
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            if (bytes_read < 0) perror("read");
            break;
        }
        buffer[bytes_read] = '\0'; // ���������� ������
        to_uppercase(buffer);
        printf("��������: %s\n", buffer);
    }

    // �������� �������
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

