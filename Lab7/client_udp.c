#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 5432
#define CHUNK_SIZE 512

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
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    int sockfd, bytes_received;
    char buffer[CHUNK_SIZE];
    FILE *output_file;
    uint16_t expected_seq = 0;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_IP> <output_file>\n", argv[0]);
        exit(1);
    }

    output_file = fopen(argv[2], "wb");
    if (!output_file) {
        perror("Error opening file");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP");
        exit(1);
    }

    char request[] = "GET";
    sendto(sockfd, request, strlen(request), 0, (struct sockaddr *)&server_addr, addr_len);

    printf("Request sent. Waiting for file...\n");

    while (1) {
        DataPacket packet;
        bytes_received = recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&server_addr, &addr_len);

        if (bytes_received < 0) {
            perror("Receive failed");
            break;
        }

        if (strncmp(packet.data, "BYE", 3) == 0) {
            printf("File received successfully.\n");
            break;
        }

        uint16_t seq_num = ntohs(packet.seq_num);
        uint16_t block_size = ntohs(packet.block_size);

        if (seq_num == expected_seq) {
            fwrite(packet.data, 1, block_size, output_file);
            expected_seq++;
        }

        AckPacket ack;
        ack.type = 1;
        ack.seq_num = htons(seq_num);
        sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&server_addr, addr_len);
        printf("Sent ACK for %d\n", seq_num);
    }

    fclose(output_file);
    close(sockfd);
    return 0;
}
