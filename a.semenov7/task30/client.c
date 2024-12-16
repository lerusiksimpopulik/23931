#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./my_socket"
#define BUFFER_SIZE 256

int main() {
    int client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    // �������� ������
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // ��������� ������ ������
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // ����������� � �������
    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // ������ ������ �� ������������ � �������� �� �������
    printf("������� ����� (��� ������ ������� 'exit'):\n");
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        if (strncmp(buffer, "exit", 4) == 0) {
            break; // ����� �� ���������
        }
        write(client_fd, buffer, strlen(buffer));
    }

    // �������� ������
    close(client_fd);
    return 0;
}

