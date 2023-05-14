#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <poll.h>
#include <time.h>
#include <sys/mman.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>


#define SOCKET_PATH "/tmp/file.txt"
#define BUFFER_SIZE 4096
#define FIFO_NAME "myfifo"
#define FILE_NAME "data.txt"
#define FILE_SIZE 100*1024*1024
#define MAX_EVENTS 2
// #define MAX_EVENTS 1


// partA
void clientA(char *serverIp, int serverPort);
void serverA(int serverPort);

// partB

void serverB(int serverPort, int quiet);
void s_ipv4_tcp(int serverPort, int quiet);
void s_ipv6_tcp(int serverPort, int quiet);
void s_ipv4_udp(int serverPort, int quiet);
void s_ipv6_udp(int serverPort, int quiet);
void s_uds_dgram(int quiet);
void s_uds_stream(int quiet);
void s_mmap(char *filename, int quiet);
void s_pipe(char *filename, int quiet);

void clientB(char *serverIp, int serverPort, char *type, char *param);
FILE* generate_data_to_file();
unsigned long hash(FILE *file);
void c_ipv4_tcp(char *serverIp, int serverPort);
void c_ipv6_tcp(char *serverIp, int serverPort);
void c_ipv4_udp(char *serverIp, int serverPort);
void c_ipv6_udp(char *serverIp, int serverPort);
void c_uds_dgram(char *serverIp, int serverPort);
void c_uds_stream();
void c_mmap(char* param);
void c_pipe(char* param , FILE* fp);

#endif