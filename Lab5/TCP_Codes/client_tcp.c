/*
Name: Tirthraj Raval
Enrolment: AU2240079
Email: tirthraj.r@ahduni.edu.in
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define P 5433
#define B 1024

int main(int c, char *v[]) {
    if (c != 2) {
        printf("Use: %s <ip>\n", v[0]);
        return 1;
    }

    int s;
    struct sockaddr_in a;
    char d[B];

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        printf("Socket Creation Failed!\n");
        return 1;
    }

    a.sin_family = AF_INET;
    a.sin_port = htons(P);
    inet_pton(AF_INET, v[1], &a.sin_addr);

    if (connect(s, (struct sockaddr*)&a, sizeof(a)) < 0) {
        printf("Connection Failed\n");
        return 1;
    }

    printf("Connextion Done\n");

    FILE *f = fopen("recieved_data.txt", "wb");
    if (!f) {
        printf("No file\n");
        return 1;
    }

    int r;
    while ((r = recv(s, d, B, 0)) > 0) {
        fwrite(d, 1, r, f);
    }

    printf("File saved as recieved_data.txt\n");

    fclose(f);
    close(s);
    return 0;
}
