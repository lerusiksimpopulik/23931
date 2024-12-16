#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#define SOCKET_PATH "./socket"
#define BUFFER_SIZE 256

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds;
    
    // �������� ������
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // �������� ������� ������, ���� �� ����������
    unlink(SOCKET_PATH);

    // ��������� ������ ������
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // �������� ������ � ������
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // ������������� �������� ����������
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("������ �������, ������� ����������� ��������...\n");

    // ������ ��� �������� ������������ ��������
    int client_fds[10];
    int client_count = 0;

    while (1) {
        // ��������� ��������� ������������
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;

        // ���������� ������������ ��������
        for (int i = 0; i < client_count; i++) {
            FD_SET(client_fds[i], &read_fds);
            if (client_fds[i] > max_fd) {
                max_fd = client_fds[i];
            }
        }

        // �������� ���������� �� �������
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // �������� ������ �����������
        if (FD_ISSET(server_fd, &read_fds)) {
            if ((client_fd = accept(server_fd, NULL, NULL)) < 0) {
                perror("accept");
                continue;
            }

            // ���������� ������ ������� � ������
            client_fds[client_count++] = client_fd;
            printf("������ ���������. ������������ ��������: %d\n", client_count);
        }

        // ��������� �������� ��������� �� ��������
        for (int i = 0; i < client_count; i++) {
            if (FD_ISSET(client_fds[i], &read_fds)) {
                ssize_t bytes_read = read(client_fds[i], buffer, sizeof(buffer) - 1);
                if (bytes_read <= 0) {
                    // ������ ��� ������ ����������
                    close(client_fds[i]);
                    client_fds[i] = client_fds[--client_count]; // �������� �������
                    printf("������ ��������. ������������ ��������: %d\n", client_count);
                } else {
                    buffer[bytes_read] = '\0'; // ���������� ������
                    to_uppercase(buffer);
                    printf("�������� �� �������: %s", buffer);
                }
            }
        }

        // �������� �� ���������� ������������ ��������
        if (client_count == 0) {
            printf("��� ������������ ��������. ������ ��������� ������.\n");
            break; // ����� �� �����, ���������� ������ �������
        }
    }

    // �������
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

