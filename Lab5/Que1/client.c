#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define P 5433
#define B 256
#define FN "recieved.txt"

int main(int c, char *v[]) {
    struct hostent *h;
    struct sockaddr_in a;
    char b[B];
    int s;
    FILE *fp;

    if (c != 2) {
        printf("Use: %s <server>\n", v[0]);
        return 1;
    }

    h = gethostbyname(v[1]);
    if (!h) {
        printf("No host\n");
        return 1;
    }

    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    memcpy(&a.sin_addr, h->h_addr_list[0], h->h_length);
    a.sin_port = htons(P);

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        printf("No sock\n");
        return 1;
    }

    if (connect(s, (struct sockaddr *)&a, sizeof(a)) < 0) {
        printf("No connect\n");
        return 1;
    }
    printf("Connected\n");

    printf("Type 'GET' to get file: ");
    fgets(b, B, stdin);
    b[strcspn(b, "\n")] = 0; // Remove newline

    send(s, b, strlen(b), 0);
    printf("Sent\n");

    fp = fopen(FN, "w");
    if (!fp) {
        printf("No file\n");
        return 1;
    }

    while (recv(s, b, sizeof(b), 0) > 0) {
        fputs(b, fp);
    }

    printf("Got it! Saved as %s\n", FN);
    fclose(fp);
    close(s);

    return 0;
}
