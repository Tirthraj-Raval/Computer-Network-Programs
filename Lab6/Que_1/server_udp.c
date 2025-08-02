/* Computer Networks
   UDP server
   Team: Tirthraj Raval, Naitik Shah
*/
/*
Usage: ./server_udp test_song.mp3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_PORT 5432
#define CHUNK_SIZE 512  
#define DELAY_MS 30     

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int sockfd, bytes_read;
    char buffer[CHUNK_SIZE];
    FILE *file;
    struct timespec sleep_time;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file_to_stream>\n", argv[0]);
        exit(1);
    }

    file = fopen(argv[1], "rb");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Sock creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("Server ready. Waiting for client...\n");

    while (1) {
        memset(buffer, 0, CHUNK_SIZE);
        recvfrom(sockfd, buffer, CHUNK_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        printf("Client connected, streamm starting...\n");

        if (strncmp(buffer, "GET", 3) == 0) {
            while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
                if (sendto(sockfd, buffer, bytes_read, 0, (struct sockaddr *)&client_addr, client_addr_len) < 0) {
                    perror("Error sending packt");
                    break;
                }
                
                
                sleep_time.tv_sec = 0;
                sleep_time.tv_nsec = DELAY_MS * 1000000L;  
                nanosleep(&sleep_time, NULL);
            }

            strcpy(buffer, "BYE");
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_addr_len);
            printf("File transmission completeee.\n");
            fseek(file, 0, SEEK_SET); 
        }
    }

    fclose(file);
    close(sockfd);
    return 0;
}
