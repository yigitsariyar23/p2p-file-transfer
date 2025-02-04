#ifndef FILE_EXCHANGE_H
#define FILE_EXCHANGE_H

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

#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg);
void run_server();
void run_client(const char *server_ip, const char *file_path);

#endif // FILE_EXCHANGE_H
