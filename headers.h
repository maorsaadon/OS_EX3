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
void s_mmap(int serverPort, int quiet);
void s_pipe(int quiet);

void clientB(char *serverIp, int serverPort, char *type, char *param);
void generate_data_to_file();
int ipv4_tcp_client(const char *server_address, int server_port);
void ipv6_tcp_client(const char *server_address, int server_port);
int ipv4_udp_client(const char *server_address, int server_port);
int ipv6_udp_client(const char *server_address, int server_port);
int uds_dgram_client(const char *server_address, int server_port);
int uds_stream_client(const char *server_address, int erver_port);
int mmap_client(const char *server_address, int server_port);
int pipe_client(const char *server_address, int server_port);

#endif