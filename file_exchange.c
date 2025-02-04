#include "file_exchange.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <arpa/inet.h>
#endif
#include <fcntl.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif
#include <sys/types.h>

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// Server code
void run_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        error("Socket creation failed");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("Bind failed");
    }

    if (listen(server_fd, 3) < 0) {
        error("Listen failed");
    }

    printf("Server is listening on port %d\n", PORT);

    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        error("Accept failed");
    }

    printf("Connection established with client\n");

    // Receive file name
    read(new_socket, buffer, BUFFER_SIZE);
    printf("Receiving file: %s\n", buffer);

    int file_fd = open(buffer, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd < 0) {
        error("Failed to open file");
    }

    // Receive file data
    ssize_t bytes_read;
    while ((bytes_read = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
        write(file_fd, buffer, bytes_read);
    }

    printf("File received successfully\n");
    close(file_fd);
    close(new_socket);
    close(server_fd);
}

// Client code
void run_client(const char *server_ip, const char *file_path) {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error("Socket creation failed");
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        error("Invalid address or address not supported");
    }

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        error("Connection failed");
    }

    printf("Connected to server\n");

    // Send file name
    const char *file_name = strrchr(file_path, '/');
    file_name = (file_name) ? file_name + 1 : file_path;
    send(sock, file_name, strlen(file_name), 0);

    // Send file data
    int file_fd = open(file_path, O_RDONLY);
    if (file_fd < 0) {
        error("Failed to open file");
    }

    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        send(sock, buffer, bytes_read, 0);
    }

    printf("File sent successfully\n");
    close(file_fd);
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [server|client] [file_path or server_ip]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "server") == 0) {
        run_server();
    } else if (strcmp(argv[1], "client") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage for client: %s client <server_ip> <file_path>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        run_client(argv[2], argv[3]);
    } else {
        fprintf(stderr, "Invalid mode. Use 'server' or 'client'.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
