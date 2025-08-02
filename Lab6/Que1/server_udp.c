/*
Name: Tirthraj Raval
Enrolment: AU2240079
Email: tirthraj.r@ahduni.edu.in
*/

/*
Usage: ./server_udp test_song.mp3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5433
#define BUFF_SIZE 1024

int main() {
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

    printf("Listening...\n");

    recvfrom(sock, b, sizeof(b), 0, (struct sockaddr *)&cli, &len);
    if (strcmp(b, "GET") != 0) {
        printf("Invalid request received\n");
        close(sock);
        return 1;
    }
    printf("Got GET request.\n");

    recvfrom(sock, b, sizeof(b), 0, (struct sockaddr *)&cli, &len);
    b[strcspn(b, "\n")] = 0; 
    printf("Client requested file: %s\n", b);

    f = fopen(b, "r");
    if (!f) {
        sendto(sock, "File not found!", 15, 0, (struct sockaddr *)&cli, len);
        printf("File not found!\n");
    } else {
        while (fgets(b, BUFF_SIZE, f)) {
            sendto(sock, b, strlen(b), 0, (struct sockaddr *)&cli, len);
        }
        sendto(sock, "BYE", 3, 0, (struct sockaddr *)&cli, len);
        printf("Sent file successfully.\n");
        fclose(f);
    }

    close(sock);
    return 0;
}
