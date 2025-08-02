#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>  

#define SERVER_PORT 5432
#define CHUNK_SIZE 512
#define TIMEOUT_SEC 2  

typedef struct {
    uint8_t type;         
    uint16_t seq_num;     
    uint16_t block_size;  
    char data[CHUNK_SIZE];
} DataPacket;

typedef struct {
    uint8_t type;         
    uint16_t seq_num;     
} AckPacket;

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int sockfd, bytes_read;
    char buffer[CHUNK_SIZE];
    FILE *file;
    uint16_t seq_num = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file_to_send>\n", argv[0]);
        exit(1);
    }

    file = fopen(argv[1], "rb");
    if (!file) {
        perror("File error");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        exit(1);
    }

    printf("Server is ready...\n");

    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
    printf("Received request from client, sending file: %s\n", argv[1]);

    struct timeval timeout = {TIMEOUT_SEC, 0};  
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        DataPacket packet;
        packet.type = 3;
        packet.seq_num = htons(seq_num);
        packet.block_size = htons(bytes_read);
        memcpy(packet.data, buffer, bytes_read);

        while (1) {
            sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, client_len);
            printf("Sent packet %d\n", seq_num);

            AckPacket ack;
            if (recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, &client_len) > 0 &&
                ntohs(ack.seq_num) == seq_num) {
                printf("ACK received for %d\n", seq_num);
                break;
            } else {
                printf("Timeout! Resending packet %d\n", seq_num);
            }
        }

        seq_num++;
    }

    strcpy(buffer, "BYE");
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len);
    printf("File transfer complete.\n");

    fclose(file);
    close(sockfd);
    return 0;
}
