#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./socket"
#define BUFFER_SIZE 256

int main() {
    int sock;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE] = {'a', 'b', 'c', '\n'};

    // �������� ������
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // ��������� ������ ������
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // ����������� � �������
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("������� ����� (��� ������ ������� 'exit'):\n");
    while (1) {
        //fgets(buffer, sizeof(buffer), stdin);
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }
	sleep(2);
        write(sock, buffer, strlen(buffer));
    }

    // �������� ������
    close(sock);
    return 0;
}

