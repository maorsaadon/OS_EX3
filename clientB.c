
#include "headers.h"

#define SOCKET_PATH "/tmp/file.txt"
#define BUFFER_SIZE 4096
#define FILE_NAME "file.txt"
#define FIFO_NAME "myfifo"
#define MAX_EVENTS 2

void generate_data_to_file()
{

    // Write the data to a file
    FILE *file = fopen("data.txt", "w+");
    if (file == NULL)
    {
        printf("Error: Unable to open file for writing!\n");
        exit(1);
    }

    char *data = (char *)malloc(1024);
    if (data == NULL)
    {
        printf("Error: Unable to allocate memory!\n");
        exit(1);
    }

    int counter = 0;
    while (counter <= 100 * 1024 * 1024)
    {
        // Seed the random number generator
        srand(time(NULL));
        for (int i = 0; i < 1024; i++)
        {
            data[i] = rand() % 256; // Generate a random byte
        }

        // Fill the data with random values
        fwrite(data, 1, 1024, file);
        counter = 1024;
    }

    fclose(file);

    free(data);

    return;
}

int ipv4_tcp_client(const char *server_address, const char *server_port)
{
    int client_socket, bytes_sent, err;
    struct sockaddr_in server_addr;
    FILE *fp;
    char buffer[BUFFER_SIZE];
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(server_port));
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    err = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (err == -1)
    {
        perror("connect");
        close(client_socket);
        return -1;
    }

    // Open input file for reading
    int file_fd = open("data.txt", O_RDONLY);
    if (file_fd == -1)
    {
        perror("open failed");
        close(client_socket);
        return -1;
    }

    while ((bytes_sent = read(file_fd, buffer, BUFFER_SIZE)) > 0)
    {
        if (write(client_socket, buffer, bytes_sent) == -1)
        {
            perror("write failed");
            close(client_socket);
            close(file_fd);
            return -1;
        }
    }

    if (bytes_sent == -1)
    {
        perror("read failed");
        close(client_socket);
        close(file_fd);
        return -1;
    }

    close(file_fd);
    close(client_socket);
    return 0;
}

//----------------------------------------------------------------------

void c_ipv6_tcp(const char *server_address, const char *server_port)
{
    int client_socket, err;
    struct sockaddr_in6 server_addr;
    char buffer[1024];
    FILE *fp;
    size_t nbytes, total_bytes = 0;

    client_socket = socket(AF_INET6, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, server_address, &server_addr.sin6_addr);
    server_addr.sin6_port = htons(atoi(server_port));

    err = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        perror("connect");
        exit(1);
    }

    fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    while ((nbytes = fread(buffer, sizeof(char), sizeof(buffer), fp)) > 0)
    {
        total_bytes += nbytes;
        err = send(client_socket, buffer, nbytes, 0);
        if (err == -1)
        {
            perror("send");
            exit(1);
        }
    }

    fclose(fp);
    close(client_socket);
}

//----------------------------------------------------------

int c_ipv4_udp(const char *server_address, const char *server_port)
{
    int client_socket, err;
    struct sockaddr_in server_addr;
    FILE *fp;
    char buffer[1024];
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(server_port));
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }
    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = client_socket;
    fds[0].events = POLLOUT;
    fds[1].fd = fileno(fp);
    fds[1].events = POLLIN;
    int nfds = 2;
    while (1)
    {
        int ret = poll(fds, nfds, -1);
        if (ret == -1)
        {
            perror("poll");
            exit(1);
        }
        if (fds[0].revents & POLLOUT)
        {
            if (fgets(buffer, sizeof(buffer), fp) != NULL)
            {
                err = sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
                if (err == -1)
                {
                    perror("sendto");
                    exit(1);
                }
            }
            else
            {
                close(fds[1].fd);
                nfds--;
            }
        }
        if (fds[1].revents & POLLIN)
        {
            // Handle input from stdin if required
            // ...
        }
        if (nfds == 0)
        {
            break;
        }
    }
    strcpy(buffer, "exit\n");
    err = sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (err == -1)
    {
        perror("sendto");
        exit(1);
    }
    close(client_socket);

    return 0;
}

//----------------------------------------------------------------------------

int c_ipv6_udp(const char *server_address, const char *server_port)
{
    int client_socket, err;
    struct sockaddr_in6 server_addr;
    FILE *fp;
    char buffer[1024];
    client_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        exit(1);
    }
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(server_port);
    inet_pton(AF_INET6, server_address, &(server_addr.sin6_addr));
    fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }
    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = client_socket;
    fds[0].events = POLLOUT;
    while (1)
    {
        int ret = poll(fds, MAX_EVENTS, -1);
        if (ret == -1)
        {
            perror("poll");
            exit(1);
        }
        if (fds[0].revents & POLLOUT)
        {
            memset(buffer, 0, sizeof(buffer));
            if (fgets(buffer, sizeof(buffer), fp) != NULL)
            {
                err = sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in6));
                if (err == -1)
                {
                    perror("sendto");
                    exit(1);
                }
            }
            else
            {
                strcpy(buffer, "exit\n");
                for (int i = 0; i < 100 * 1024 * 1024 / strlen(buffer); i++)
                {
                    err = sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in6));
                    if (err == -1)
                    {
                        perror("sendto");
                        exit(1);
                    }
                }
                break;
            }
        }
    }
    fclose(fp);
    close(client_socket);
    return 0;
}

//-------------------------------------------------------

int c_uds_dgram(const char *server_address, const char *server_port)
{
    int client_socket, err;
    struct sockaddr_un server_addr;
    FILE *fp;
    char buffer[1024];
    client_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        exit(1);
    }
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        err = sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
        if (err == -1)
        {
            perror("sendto");
            exit(1);
        }
    }
    fclose(fp);
    strcpy(buffer, "exit\n");
    err = sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
    if (err == -1)
    {
        perror("sendto");
        exit(1);
    }
    close(client_socket);
    return 0;
}

//-------------------------------------------------------

int c_uds_stream(const char *server_address, const char *server_port)
{
    int client_socket, err;
    struct sockaddr_un server_addr;
    FILE *fp;
    char buffer[1024];
    struct pollfd fds[1];
    int timeout = 5000;
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("socket");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    err = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
    if (err == -1)
    {
        perror("connect");
        exit(1);
    }
    fds[0].fd = client_socket;
    fds[0].events = POLLOUT;
    fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        err = poll(fds, 1, timeout);
        if (err == -1)
        {
            perror("poll");
            exit(1);
        }
        else if (err == 0)
        {
            continue;
        }

        if (fds[0].revents & POLLOUT)
        {
            err = send(client_socket, buffer, strlen(buffer), 0);
            if (err == -1)
            {
                perror("send");
                exit(1);
            }
        }
    }
    fclose(fp);
    close(client_socket);

    return 0;
}

//-----------------------------------------------------------

int c_mmap(const char *server_address, const char *server_port)
{

    struct sockaddr_in serv_addr;
    const char *filename = "file.txt";
    struct stat filestat;
    int fd = open("data.txt", O_RDONLY);
    if (fd < 0)
    {
        perror("open");
    }
    if (fstat(fd, &filestat) < 0)
    {
        perror("fstat");
    }
    char *filedata = mmap(0, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (filedata == MAP_FAILED)
    {
        perror("mapp");
    }
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
    }
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    int n = sendto(sockfd, filedata, filestat.st_size, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0)
    {
        perror("sendto");
    }
    close(fd);
    close(sockfd);
    return 0;
}

//------------------------------------------------------------------------------

int c_pipe(const char *server_address, const char *server_port)
{
    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    FILE *file;
    file = fopen("data.txt", "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1)
    {
        perror("Failed to open named pipe");
        exit(EXIT_FAILURE);
    }
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        write(fd, buffer, bytes_read);
    }
    fclose(file);
    close(fd);
    return 0;
}

void clientB(char *serverIp, int serverPort, char *type, char *param)
{

    // create file that 100mb size
    generate_data_to_file();

    //--------------sending choice to server-------------

    int client_fd;
    struct sockaddr_in server_addr;

    // Create a socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Prepare the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(serverIp); // Server IP address
    server_addr.sin_port = htons(8000);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(serverPort)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("message1: %s\n", type);
    printf("message2: %s\n", param);

    // Send data to the server
    if (send(client_fd, type, strlen(type), 0) == -1)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    if (send(client_fd, param, strlen(param), 0) == -1)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }

    // Close the connection
    close(client_fd);

    sleep(1);
    //------------------------------------------------

    if (strcmp(type, "ipv4") == 0 && (strcmp(type, "tcp")) == 0)
    {
        printf("client open\n");
        ipv4_tcp_client(serverIp, serverPort);
    }
    else if (strcmp(type, "ipv4") == 0 && (strcmp(type, "udp")) == 0)
    {

        c_ipv4_udp(serverIp, serverPort);
    }
    else if (strcmp(type, "ipv6") == 0 && (strcmp(type, "tcp")) == 0)
    {

        c_ipv6_tcp(serverIp, serverPort);
    }
    else if (strcmp(type, "ipv6") == 0 && (strcmp(type, "udp")) == 0)
    {

        c_ipv6_udp(serverIp, serverPort);
    }

    else if (strcmp(type, "mmap") == 0 && (strcmp(type, "filename")) == 0)
    {

        c_mmap(serverIp, serverPort);
    }
    else if (strcmp(type, "pipe") == 0 && (strcmp(type, "filename")) == 0)
    {

        c_pipe(serverIp, serverPort);
    }
    else if (strcmp(type, "uds") == 0 && (strcmp(type, "dgram")) == 0)
    {

        c_uds_dgram(serverIp, serverPort);
    }
    else if (strcmp(type, "uds") == 0 && (strcmp(type, "stream")) == 0)
    {

        c_uds_stream(serverIp, serverPort);
    }
}
