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

#define PORT 5433
#define BUFF_SIZE 1024

int main(int c, char *v[]) {
    if (c != 3 || strcmp(v[1], "-f")) {
        printf("Use: %s -f <file>\n", v[0]);
        return 1;
    }

    int sock;
    struct sockaddr_in srv, cli;
    char b[BUFF_SIZE];
    socklen_t len = sizeof(cli);
    FILE *f;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("No sock\n");
        return 1;
    }

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        printf("No bind\n");
        return 1;
    }

    recvfrom(sock, b, sizeof(b), 0, (struct sockaddr *)&cli, &len);

    f = fopen(v[2], "r");
    if (!f) {
        sendto(sock, "No file", 7, 0, (struct sockaddr *)&cli, len);
        printf("File?\n");
    } else {
        while (fgets(b, BUFF_SIZE, f)) {
            sendto(sock, b, strlen(b), 0, (struct sockaddr *)&cli, len);
        }
        sendto(sock, "BYE", 3, 0, (struct sockaddr *)&cli, len);
        fclose(f);
    }

    close(sock);
}
