/* Computer Networks
   UDP client
   Team: Tirthraj Raval, Naitik Shah
*/
/*
Usage ./client_udp 127.0.0.1 output.mp3
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
#define BUF_SIZE 512   
#define DELAY_MS 30    

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    int sockfd, bytes_received;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[BUF_SIZE];
    FILE *output_file = NULL;
    FILE *ffmpeg_pipe = NULL;
    struct timespec sleep_time;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_IP> <output_file>\n", argv[0]);
        exit(1);
    }

    output_file = fopen(argv[2], "wb");
    if (!output_file) {
        perror("Error opening file for writing");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Sock creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid server Ip");
        exit(1);
    }

    strcpy(buffer, "GET");
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);

    printf("Receiving file and streamiing...\n");

    char command[256];
    snprintf(command, sizeof(command), "ffplay -nodisp -autoexit -");
    ffmpeg_pipe = popen(command, "w");
    if (!ffmpeg_pipe) {
        perror("Error opening FFmpeg for streaming");
        fclose(output_file);
        exit(1);
    }

    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = DELAY_MS * 1000000L; 

    while (1) {
        bytes_received = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Receive failed");
            break;
        }

        if (strncmp(buffer, "BYE", 3) == 0) {
            printf("File received successfullyy.\n");
            break;
        }

        fwrite(buffer, 1, bytes_received, output_file);
        fwrite(buffer, 1, bytes_received, ffmpeg_pipe);
        fflush(ffmpeg_pipe);

        nanosleep(&sleep_time, NULL);
    }

    fclose(output_file);
    pclose(ffmpeg_pipe);
    close(sockfd);
    return 0;
}
