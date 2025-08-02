#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define P 5433
#define Q 5
#define B 1024

int main(int c, char *v[]) {
    struct sockaddr_in a, ca;
    char b[B];
    socklen_t l;
    int sfd, cfd;
    char *f = "sample.txt";

    if (c == 2) {
        f = v[1];
    }

    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(P);

    sfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        printf("No socket created\n");
        return 1;
    }

    if (bind(sfd, (struct sockaddr *)&a, sizeof(a)) < 0) {
        printf("No bind\n");
        return 1;
    }
    printf("Listening...\n");

    listen(sfd, Q);

    while (1) {
        l = sizeof(ca);
        cfd = accept(sfd, (struct sockaddr *)&ca, &l);
        if (cfd < 0) {
            printf("Connection Faild\n");
            continue;
        }
        printf("Got one request!\n");

        memset(b, 0, sizeof(b));
        if (recv(cfd, b, sizeof(b), 0) > 0) {
            if (strcmp(b, "GET") == 0) {
                FILE *fp = fopen(f, "rb");
                if (!fp) {
                    printf("No file found!\n");
                    send(cfd, "No file found!", 7, 0);
                } else {
                    while (!feof(fp)) {
                        int r = fread(b, 1, B, fp);
                        send(cfd, b, r, 0);
                    }
                    fclose(fp);
                    printf("File Successfully sent\n");
                }
            } else {
                printf("Bad request\n");
            }
        }
        close(cfd);
    }

    close(sfd);
    return 0;
}
