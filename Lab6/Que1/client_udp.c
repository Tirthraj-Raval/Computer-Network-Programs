#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 5433
#define BUFF_SIZE 1024
#define TIMEOUT_SEC 5

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server-ip> <output-file>\n", argv[0]);
        return 1;
    }

    int sock;
    struct sockaddr_in srv;
    socklen_t len = sizeof(srv);
    char buffer[BUFF_SIZE];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &srv.sin_addr);

    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    sendto(sock, "GET", 3, 0, (struct sockaddr *)&srv, len);

    long file_size;
    recvfrom(sock, &file_size, sizeof(file_size), 0, (struct sockaddr *)&srv, &len);
    printf("Receiving file of size: %ld bytes\n", file_size);

    FILE *file = fopen(argv[2], "wb");
    if (!file) {
        perror("File creation failed");
        close(sock);
        return 1;
    }

    long received_bytes = 0;
    while (received_bytes < file_size) {
        ssize_t bytes_received = recvfrom(sock, buffer, BUFF_SIZE, 0, (struct sockaddr *)&srv, &len);
        if (bytes_received < 0) {
            printf("Timeout or error receiving file.\n");
            break;
        }

        if (strncmp(buffer, "EOF", 3) == 0)
            break;

        fwrite(buffer, 1, bytes_received, file);
        received_bytes += bytes_received;
    }

    printf("File received successfully.\n");

    fclose(file);
    close(sock);
    return 0;
}
