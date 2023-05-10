
#include "headers.h"

#define MAX_EVENTS 1
#define SOCKET_PATH "/tmp/file.txt"
#define BUFFER_SIZE 4096
#define FILE_NAME "file.txt"
#define FIFO_NAME "myfifo"

int s_ipv4_tcp(int serverPort, int quiet)
{
    // Create socket
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 1)
            printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else if (quiet == 1)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 1)
            printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress = {0}, clientAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = serverPort;

    // Bind socket to address and port
    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        if (quiet == 1)
            printf("Bind failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("executed Bind() successfully\n");

    // Listen for incoming connections
    int listenResult = listen(serverFd, 1);
    if (listenResult == -1)
    {
        if (quiet == 1)
            printf("listen() failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("Waiting for incoming TCP-connections...\n");

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 1)
            printf("listen failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("A new client connection accepted\n");

    char buffer[1024] = {0};
    FILE *fp;

    fp = fdopen(clientFd, "r");
    if (fp == NULL)
    {
        if (quiet == 1)
            printf("fdopen failed with error code : %d", errno);
        exit(1);
    }

    FILE *output_file = fopen("ipv4_tcp.txt", "w");
    if (output_file == NULL)
    {
        if (quiet == 1)
            printf("fopen failed with error code : %d", errno);
        exit(1);
    }

    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        fputc(c, output_file);
    }

    fclose(fp);
    close(clientFd);
    close(serverFd);
    return 0;
}

void s_ipv6_tcp(int serverPort, int quiet)
{
    // Create socket
    int serverFd = socket(AF_INET6, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 1)
            printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else if (quiet == 1)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 1)
            printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in6 serverAddress = {0}, clientAddress = {0};
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port = serverPort;
    serverAddress.sin6_addr = in6addr_any;

    // Bind socket to address and port
    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        if (quiet == 1)
            printf("Bind failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("executed Bind() successfully\n");

    // Listen for incoming connections
    int listenResult = listen(serverFd, 1);
    if (listenResult == -1)
    {
        if (quiet == 1)
            printf("listen() failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("Waiting for incoming TCP-connections...\n");

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(struct sockaddr_in6);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 1)
            printf("listen failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("A new client connection accepted\n");

    char ip_str[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &clientAddress.sin6_addr, ip_str, INET6_ADDRSTRLEN);

    unsigned long long checksum = 0;
    char buffer[1024] = {0};
    FILE *fp;

    fp = fdopen(clientFd, "r");
    if (fp == NULL)
    {
        if (quiet == 1)
            printf("fdopen failed with error code : %d", errno);
        exit(1);
    }

    size_t nbytes;
    while (1)
    {
        nbytes = fread(buffer, sizeof(char), sizeof(buffer), fp);
        if (nbytes >= 0)
        {
            break;
        }
    }

    fclose(fp);
    close(clientFd);
    close(serverFd);
}

int s_ipv4_udp(int serverPort, int quiet)
{
    // Create socket
    int serverFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 1)
            printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else if (quiet == 1)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 1)
            printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress = {0}, clientAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = serverPort;

    memset(&(serverAddress.sin_zero), '\0', 8);
    // Bind socket to address and port
    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        if (quiet == 1)
            printf("Bind failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("executed Bind() successfully\n");

    char buffer[1024];
    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);

    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = serverFd;
    fds[0].events = POLLIN;
    while (1)
    {
        int ret = poll(fds, MAX_EVENTS, -1);
        if (ret == -1)
        {
            perror("poll");
            exit(1);
        }
        if (fds[0].revents & POLLIN)
        {
            ssize_t num_bytes = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (num_bytes == -1)
            {
                if (quiet == 1)
                    printf("recv failed with error code : %d", errno);
                exit(-1);
            }

            if (!strcmp(buffer, "exit\n"))
            {
                close(serverFd);
                return 0;
            }
            else
            {
                FILE *fp = fopen("ipv4_udp.txt", "ab");
                if (fp == NULL)
                {
                    if (quiet == 1)
                        printf("fopen failed with error code : %d", errno);
                    exit(1);
                }

                int bytes_received = 0;
                int total_bytes_received = 0;
                int max_bytes = 100 * 1024 * 1024; // 100MB
                while (bytes_received == 0)
                {
                    bytes_received = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
                    if (bytes_received == -1)
                    {
                        if (quiet == 1)
                            printf("recv failed with error code : %d", errno);
                        exit(-1);
                    }
                    fwrite(buffer, bytes_received, 1, fp);
                    total_bytes_received += bytes_received;
                    if (bytes_received == 0)
                    {
                        break;
                    }
                }

                fclose(fp);
                break;
            }
        }
    }
    return 0;
}

int s_ipv6_udp(int serverPort, int quiet)
{

    int serverFd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 1)
            printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else if (quiet == 1)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 1)
            printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    struct sockaddr_in6 serverAddress = {0}, clientAddress = {0};
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port = serverPort;
    serverAddress.sin6_addr = in6addr_any;

    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in6));
    if (bindResult == -1)
    {
        if (quiet == 1)
            printf("Bind failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("executed Bind() successfully\n");

    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = serverFd;
    fds[0].events = POLLIN;

    char buffer[1024] = {0};
    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(struct sockaddr_in6);
    int running = 1;

    FILE *fp = fopen("ipv6_udp.txt", "w");
    if (fp == NULL)
    {
        if (quiet == 1)
            printf("fopen failed with error code : %d", errno);
        exit(1);
    }

    while (running)
    {
        int pull = poll(fds, MAX_EVENTS, -1);
        if (pull == -1)
        {
            if (quiet == 1)
                printf("Pull failed with error code : %d", errno);
            exit(-1);
        }

        if (fds[0].revents & POLLIN)
        {

            int bytes_received = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 1)
                    printf("recv failed with error code : %d", errno);
                exit(-1);
            }

            fwrite(buffer, sizeof(char), bytes_received, fp);

            if (ftell(fp) >= 100000000)
                running = 0;

            if (!strcmp(buffer, "exit\n"))
                running = 0;
        }
    }

    fclose(fp);
    close(serverFd);
    return 0;
}

int s_uds_dgram(int quiet)
{
    int serverFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 1)
            printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else if (quiet == 1)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 1)
            printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    struct sockaddr_un serverAddress = {0}, clientAddress = {0};
    unlink(SOCKET_PATH);
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);

    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un));
    if (bindResult == -1)
    {
        if (quiet == 1)
            printf("Bind failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("executed Bind() successfully\n");

    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = serverFd;
    fds[0].events = POLLIN;

    char buffer[1024];
    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(struct sockaddr_un);

    while (1)
    {
        int pull = poll(fds, MAX_EVENTS, -1);
        if (pull == -1)
        {
            if (quiet == 1)
                printf("Pull failed with error code : %d", errno);
            exit(-1);
        }
        if (fds[0].revents & POLLIN)
        {
            int bytes_received = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 1)
                    printf("recv failed with error code : %d", errno);
                exit(-1);
            }

            if (strcmp(buffer, "exit\n") == 0)
            {
                break;
            }
        }
    }
    close(serverFd);
    unlink(SOCKET_PATH);
    return 0;
}

int s_uds_stream(int quiet)
{
    int server_socket, client_socket, err;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len;
    char buffer[1024];
    struct pollfd fds[1];
    int timeout = 5000;
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    unlink(SOCKET_PATH);
    err = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
    if (err == -1)
    {
        perror("bind");
        exit(1);
    }
    err = listen(server_socket, 1);
    if (err == -1)
    {
        perror("listen");
        exit(1);
    }
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;
    while (1)
    {
        err = poll(fds, 1, timeout);
        if (err == -1)
        {
            perror("poll");
            exit(1);
        }
        else if (err == 0)
        {
            printf("Timeout occurred\n");
            continue;
        }
        if (fds[0].revents & POLLIN)
        {
            client_addr_len = sizeof(struct sockaddr_un);
            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_socket == -1)
            {
                perror("accept");
                exit(1);
            }
            FILE *fp;
            memset(buffer, 0, sizeof(buffer));
            fp = fdopen(client_socket, "r");
            if (fp == NULL)
            {
                perror("fdopen");
                exit(1);
            }
            fclose(fp);
            break;
        }
        close(client_socket);
    }
    close(server_socket);

    return 0;
}

int s_mmap(int serverPort, int quiet)
{

    struct sockaddr_in serv_addr, client_addr;
    char buffer[BUFFER_SIZE];
    struct pollfd fds[1];
    int timeout = 5000;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
    }
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = serverPort;
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind");
    }
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    while (1)
    {
        int ret = poll(fds, 1, timeout);
        if (ret == -1)
        {
            perror("poll");
        }
        else if (ret == 0)
        {
            break;
        }
        else
        {
            if (fds[0].revents & POLLIN)
            {
                bzero(buffer, BUFFER_SIZE);
                int clientlen = sizeof(client_addr);
                int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_addr, &clientlen);
                if (n < 0)
                {
                    perror("recvfrom");
                }
            }
        }
    }
    close(sockfd);
    return 0;
}

int s_pipe(int quiet)
{

    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    mkfifo(FIFO_NAME, 0666);
    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open named pipe");
        exit(EXIT_FAILURE);
    }
    FILE *file = fopen("received_file.txt", "w");
    if (file == NULL)
    {
        perror("Failed to open file for writing");
        exit(EXIT_FAILURE);
    }
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0)
    {
        fwrite(buffer, 1, bytes_read, file);
    }
    fclose(file);
    close(fd);
    unlink(FIFO_NAME);

    return 0;
}

void serverB(int serverPort, int quiet)
{

    // Create socket
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 1)
            printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else if (quiet == 1)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 1)
            printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress = {0}, clientAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = serverPort;

    // Bind socket to address and port
    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        if (quiet == 1)
            printf("Bind failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("executed Bind() successfully\n");

    // Listen for incoming connections
    int listenResult = listen(serverFd, 1);
    if (listenResult == -1)
    {
        if (quiet == 1)
            printf("listen() failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("Waiting for incoming TCP-connections...\n");

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 1)
            printf("listen failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("A new client connection accepted\n");

    char buffer[2][1024];

    // Receive message1 from the client
    ssize_t num_bytes = recv(clientFd, buffer[0], strlen(buffer[0]) - 1, 0);
    if (num_bytes == -1)
    {
        if (quiet == 1)
            printf("recv failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }

    buffer[0][num_bytes] = '\0';
    if (quiet == 1)
        printf("Received message1: %s\n", buffer[0]);

    // Receive message2 from the client
    num_bytes = recv(clientFd, buffer[1], sizeof(buffer[1]) - 1, 0);
    if (num_bytes == -1)
    {
        if (quiet == 1)
            printf("recv failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }

    buffer[1][num_bytes] = '\0';

    if (quiet == 1)
        printf("Received message2: %s\n", buffer[1]);

    // Close the connection
    close(clientFd);
    close(serverFd);

    struct timeval start, end;

    if (strcmp(buffer[0], "ipv4") == 0 && (strcmp(buffer[1], "tcp")) == 0)
    {

        gettimeofday(&start, NULL);
        s_ipv4_tcp(serverPort, quiet);
        gettimeofday(&end, NULL);
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        long elapsed = seconds * 1000000 + microseconds;

        printf("ipv4_tcp,%ld\n", elapsed);
    }
    else if (strcmp(buffer[0], "ipv4") == 0 && (strcmp(buffer[1], "udp")) == 0)
    {
        gettimeofday(&start, NULL);
        s_ipv4_udp(serverPort, quiet);
        gettimeofday(&end, NULL);
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        long elapsed = seconds * 1000000 + microseconds;

        printf("ipv4_udp,%ld\n", elapsed);
    }
    else if (strcmp(buffer[0], "ipv6") == 0 && (strcmp(buffer[1], "tcp")) == 0)
    {
        gettimeofday(&start, NULL);
        s_ipv6_tcp(serverPort, quiet);
        gettimeofday(&end, NULL);
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        long elapsed = seconds * 1000000 + microseconds;

        printf("ipv6_tcp,%ld\n", elapsed);
    }
    else if (strcmp(buffer[0], "ipv6") == 0 && (strcmp(buffer[1], "udp")) == 0)
    {
        gettimeofday(&start, NULL);
        s_ipv6_udp(serverPort, quiet);
        gettimeofday(&end, NULL);
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        long elapsed = seconds * 1000000 + microseconds;

        printf("ipv6_udp,%ld\n", elapsed);
    }

    else if (strcmp(buffer[0], "mmap") == 0 && (strcmp(buffer[1], "filename")) == 0)
    {
        gettimeofday(&start, NULL);
        s_mmap(serverPort, quiet);
        gettimeofday(&end, NULL);
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        long elapsed = seconds * 1000000 + microseconds;

        printf("mmap,%ld\n", elapsed);
    }
    else if (strcmp(buffer[0], "pipe") == 0 && (strcmp(buffer[1], "filename")) == 0)
    {
        gettimeofday(&start, NULL);
        s_pipe(quiet);
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        long elapsed = seconds * 1000000 + microseconds;

        printf("pipe,%ld\n", elapsed);
    }
    else if (strcmp(buffer[0], "uds") == 0 && (strcmp(buffer[1], "dgram")) == 0)
    {
        gettimeofday(&start, NULL);
        s_uds_dgram(quiet);
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        long elapsed = seconds * 1000000 + microseconds;

        printf("uds_dgram,%ld\n", elapsed);
    }
    else if (strcmp(buffer[0], "uds") == 0 && (strcmp(buffer[1], "stream")) == 0)
    {
        gettimeofday(&start, NULL);
        s_uds_stream(quiet);
        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        long elapsed = seconds * 1000000 + microseconds;

        printf("uds_stream,%ld\n", elapsed);
    }
}