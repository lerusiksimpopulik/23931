#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int pipefd[2]; // ������ ��� �������� ������������ ������
    pid_t cpid;
    char buffer[BUFFER_SIZE];

    // �������� ������
    if (pipe(pipefd) == -1) {
        error("pipe");
    }

    // �������� ��������� ��������
    cpid = fork();
    if (cpid == -1) {
        error("fork");
    }

    if (cpid == 0) { // �������� �������
        close(pipefd[1]); // ��������� ����� ������ ������

        // ������ �� ������ � �������������� � ������� �������
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0'; // ��������� ������
            for (int i = 0; i < bytes_read; i++) {
                buffer[i] = toupper((unsigned char)buffer[i]); // ����������� � ������� �������
            }
            printf("���������� ����� � ������� ��������: %s", buffer);
        }

        if (bytes_read == -1) {
            error("read");
        }

        close(pipefd[0]); // ��������� ����� ������ ������
        exit(EXIT_SUCCESS);
    } else { // ������������ �������
        close(pipefd[0]); // ��������� ����� ������ ������

        // ������ ������ � �����
        const char *text = "Hello, World!\nThis is a test.\n";
        size_t text_length = strlen(text);
        ssize_t bytes_written = write(pipefd[1], text, text_length);

        if (bytes_written == -1) {
            error("write");
        }

        close(pipefd[1]); // ��������� ����� ������ ������
        wait(NULL); // ������� ���������� ��������� ��������
        exit(EXIT_SUCCESS);
    }
}
