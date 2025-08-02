/*
Name: Tirthraj Raval
Enrolment: AU2240079
Email: tirthraj.r@ahduni.edu.in
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 5433
#define BUFF_SIZE 1024
#define TIMEOUT_SEC 3

int main(int c, char *v[]) {
    if (c != 2) {
        printf("Use: %s <ip>\n", v[0]);
        return 1;
    }

    int sock;
    struct sockaddr_in srv;
    socklen_t len = sizeof(srv);
    char b[BUFF_SIZE];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("No sock\n");
        return 1;
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    inet_pton(AF_INET, v[1], &srv.sin_addr);

    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    sendto(sock, "GET", 3, 0, (struct sockaddr *)&srv, len);

    FILE *f = fopen("recieved_file.txt", "wb");
    if (!f) {
        printf("No file created due to error!\n");
        close(sock);
        return 1;
    }

    while (1) {
        ssize_t r = recvfrom(sock, b, BUFF_SIZE, 0, (struct sockaddr *)&srv, &len);
        if (r < 0) {
            printf("Timeout, stop.\n");
            break;
        }
        b[r] = '\0';

        if (strcmp(b, "BYE") == 0) break;

        fwrite(b, 1, r, f);
    }

    printf("Got file successfully.\n");

    fclose(f);
    close(sock);
    return 0;
}
