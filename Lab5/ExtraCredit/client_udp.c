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
        printf("No sockect Connection\n");
        return 1;
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    inet_pton(AF_INET, v[1], &srv.sin_addr);

    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Send GET request
    sendto(sock, "GET", 3, 0, (struct sockaddr *)&srv, len);

    printf("Enter filename: ");
    fgets(b, BUFF_SIZE, stdin);
    b[strcspn(b, "\n")] = 0; 

    sendto(sock, b, strlen(b), 0, (struct sockaddr *)&srv, len);

    FILE *f = fopen("recieved_file.txt", "wb");
    if (!f) {
        printf("No file created due to error!\n");
        close(sock);
        return 1;
    }

    int fileFound = 1;  // Assume file is found

    while (1) {
        ssize_t r = recvfrom(sock, b, BUFF_SIZE, 0, (struct sockaddr *)&srv, &len);
        if (r < 0) {
            printf("Timeout, stop.\n");
            fileFound = 0;
            break;
        }
        b[r] = '\0';

        if (strcmp(b, "File not found!") == 0) {
            printf("File not found!\n");
            fileFound = 0;
            break;
        }

        if (strcmp(b, "BYE") == 0) break;

        fwrite(b, 1, r, f);
    }

    if (fileFound)
        printf("Got file successfully.\n");

    fclose(f);
    close(sock);
    return 0;
}
