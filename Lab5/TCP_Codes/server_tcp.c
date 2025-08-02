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

#define P 5433
#define B 1024
#define DEFAULT_FILE "sample.txt"

int main(int c, char *v[]) {
    char *fn = DEFAULT_FILE;  // Default file
    if (c == 3 && strcmp(v[1], "-f") == 0) {
        fn = v[2];  // Use provided file
    } else if (c != 1) {
        printf("Use: %s -f <file>\n", v[0]);
        return 1;
    }

    FILE *fp = fopen(fn, "rb");
    int found = (fp != NULL);

    int s, cs;
    struct sockaddr_in a, ca;
    socklen_t cl = sizeof(ca);
    char d[B];

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        printf("No sock\n");
        return 1;
    }

    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(P);

    if (bind(s, (struct sockaddr*)&a, sizeof(a)) < 0) {
        printf("No bind\n");
        return 1;
    }

    listen(s, 5);
    printf("Listening...\n");

    while (1) {
        cs = accept(s, (struct sockaddr*)&ca, &cl);
        if (cs < 0) {
            printf("No conn\n");
            continue;
        }

        printf("Got one!\n");

        if (!found) {
            strcpy(d, "No file\n");
            send(cs, d, strlen(d), 0);
            printf("Told client no file.\n");
        } else {
            while (!feof(fp)) {
                int r = fread(d, 1, B, fp);
                send(cs, d, r, 0);
            }
            printf("Sent file\n");
        }

        fclose(fp);
        close(cs);
    }

    close(s);
    return 0;
}
