#include "headers.h"

/**
 * Performs IPv4 TCP server operation.
 * @param serverPort The port number to bind the server socket.
 * @param quiet A flag indicating whether to display verbose output or not.
 */
void s_ipv4_tcp(int serverPort, int quiet)
{
    // Create socket
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket\n");
        exit(-1);
    }
    else if (quiet == 0)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 0)
            perror("setsockopt() failed\n");
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
        if (quiet == 0)
            perror("Bind() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("executed Bind() successfully\n");

    // Listen for incoming connections
    int listenResult = listen(serverFd, 1);
    if (listenResult == -1)
    {
        if (quiet == 0)
            perror("listen() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("Waiting for incoming TCP-connections...\n");

    // socklen_t len_clientAddress = {0};
    socklen_t len_clientAddress = sizeof(clientAddress);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 0)
            perror("listen() failed\n");
        close(clientFd);
        exit(-1);
    }

    FILE *inputFile = fdopen(clientFd, "r");
    if (inputFile == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    FILE *outputFile = fopen("ipv4_tcp.txt", "w");
    if (outputFile == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    // calc time
    struct timeval start,end;
    gettimeofday(&start, NULL);

    int c;
    while ((c = fgetc(inputFile)) != EOF)
    {
        fputc(c, outputFile);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("ipv4_tcp , %ld\n", total_time);

    fclose(outputFile);
    fclose(inputFile);
    close(clientFd);
    close(serverFd);
}

/**
 * Performs IPv6 TCP server operation.
 * @param serverPort The port number to bind the server socket.
 * @param quiet A flag indicating whether to display verbose output or not.
 */

void s_ipv6_tcp(int serverPort, int quiet)
{
    // Create socket
    int serverFd = socket(AF_INET6, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket\n");
        exit(-1);
    }
    else if (quiet == 0)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 0)
            perror("setsockopt() failed\n");
        exit(-1);
    }

    // Prepare sockaddr_in6 structure
    struct sockaddr_in6 serverAddress = {0}, clientAddress = {0};
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port = htons(serverPort);

    // Bind socket to address and port
    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        if (quiet == 0)
            perror("Bind() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("executed Bind() successfully\n");

    // Listen for incoming connections
    int listenResult = listen(serverFd, 1);
    if (listenResult == -1)
    {
        if (quiet == 0)
            perror("listen() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("Waiting for incoming TCP-connections...\n");

    socklen_t len_clientAddress = sizeof(struct sockaddr_in6);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 0)
            perror("listen() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("A new client connection accepted\n");

    char ip_str[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &clientAddress.sin6_addr, ip_str, INET6_ADDRSTRLEN);

    char buffer[BUFFER_SIZE];
    FILE *inputFile = fdopen(clientFd, "r");
    if (inputFile == NULL)
    {
        if (quiet == 0)
            perror("fdopen() failed\n");
        exit(1);
    }

    FILE *outputFile = fopen("ipv6_tcp.txt", "w");
    if (outputFile == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    // Receive file size from the client
    long file_size;
    int bytes_read = recv(clientFd, &file_size, sizeof(file_size), 0);
    if (bytes_read == -1)
    {
        if (quiet == 0)
            perror("recv() failed\n");
        fclose(outputFile);
        fclose(inputFile);
        close(clientFd);
        close(serverFd);
        exit(-1);
    }

    // calc time
    struct timeval start = {0}, end = {0};
    gettimeofday(&start, NULL);
    
    size_t total_bytes = 0;
    while (total_bytes < file_size)
    {
        ssize_t nbytes = recv(clientFd, buffer, sizeof(buffer), 0);
        if (nbytes <= 0)
        {
            if (quiet == 0)
                perror("recv() failed\n");
            fclose(outputFile);
            fclose(inputFile);
            close(clientFd);
            close(serverFd);
            exit(-1);
        }

        total_bytes += nbytes;
        fwrite(buffer, sizeof(char), nbytes, outputFile);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("ipv6_tcp, %ld\n", total_time);

    fclose(outputFile);
    fclose(inputFile);
    close(clientFd);
    close(serverFd);
}

/**
 * Performs IPv4 UDP server operation.
 * @param serverPort The port number to bind the server socket.
 * @param quiet A flag indicating whether to display verbose output or not.
 */
void s_ipv4_udp(int serverPort, int quiet)
{
    // Create socket
    int serverFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket\n");
        exit(-1);
    }
    else if (quiet == 0)
        printf("New socket opened\n");

    int enableReuse = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        if (quiet == 0)
            perror("setsockopt() failed\n");
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress = {0}, clientAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = serverPort; // you've converted it before the function

    // Bind socket to address and port
    if (bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        if (quiet == 0)
            perror("Bind() failed");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("executed Bind() successfully\n");

    FILE *outputFile = fopen("ipv4_udp.txt", "w");
    if (outputFile == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);


    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = serverFd;
    fds[0].events = POLLIN;

    char buffer[BUFFER_SIZE] = {0};

    //calc time
    struct timeval start = {0}, end = {0};
    gettimeofday(&start, NULL);

    // Inside the while loop
    while (1)
    {
        int pull = poll(fds, MAX_EVENTS, -1);
        if (pull == -1)
        {
            if (quiet == 0)
                perror("Poll() failed\n");
            exit(-1);
        }

        if (fds[0].revents & POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            long long bytes_received = recvfrom(serverFd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 0)
                    perror("recvfrom() failed\n");

                close(serverFd);
                fclose(outputFile);
                exit(-1);
            }
            else if (bytes_received == 0)
                break;
            else
            {
                if (strstr(buffer, "exit\n") != NULL)
                    break;
                else
                {
                    fwrite(buffer, sizeof(char), bytes_received, outputFile);
                }
            }
        }
    }

    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("ipv4_udp, %ld\n", total_time);

    fclose(outputFile);
    close(serverFd);
}

/**
 * Performs IPv6 UDP server operation.
 * @param serverPort The port number to bind the server socket.
 * @param quiet A flag indicating whether to display verbose output or not.
 */
void s_ipv6_udp(int serverPort, int quiet)
{

    // Create socket
    int serverFd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket\n");
        exit(-1);
    }
    else if (quiet == 0)
        printf("New socket opened\n");

    int enableReuse = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        if (quiet == 0)
            perror("setsockopt() failed\n");
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in6 serverAddress = {0}, clientAddress = {0};
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port = serverPort;
    serverAddress.sin6_addr = in6addr_any;

    // Bind socket to address and port
    if (bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        if (quiet == 0)
            perror("Bind() failed");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("executed Bind() successfully\n");

    FILE *outputFile = fopen("ipv6_udp.txt", "w");
    if (outputFile == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);

    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = serverFd;
    fds[0].events = POLLIN;

    char buffer[BUFFER_SIZE] = {0};

    //calc time
    struct timeval start = {0}, end = {0};
    gettimeofday(&start, NULL);

    // Inside the while loop
    while (1)
    {
        int pull = poll(fds, MAX_EVENTS, -1);
        if (pull == -1)
        {
            if (quiet == 0)
                perror("Poll() failed\n");
            exit(-1);
        }

        if (fds[0].revents & POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recvfrom(serverFd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 0)
                    perror("recvfrom() failed\n");

                fclose(outputFile);
                close(serverFd);
                exit(-1);
            }
            else if (bytes_received == 0)
                break;
            else
            {
                if (strstr(buffer, "exit\n") != NULL)
                    break;
                else
                {
                    fwrite(buffer, sizeof(char), bytes_received, outputFile);
                }
            }
        }
    }

    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("ipv6_udp, %ld\n", total_time);

    fclose(outputFile);
    close(serverFd);
}

/**
 * Performs UNIX domain socket datagram server operation.
 * @param quiet A flag indicating whether to display verbose output or not.
 */
void s_uds_dgram(int quiet)
{
    int serverFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket\n");
        exit(-1);
    }
    else if (quiet == 0)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 0)
            perror("setsockopt() failed\n");
        exit(-1);
    }

    struct sockaddr_un serverAddress = {0};
    unlink(SOCKET_PATH);
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);

    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un));
    if (bindResult == -1)
    {
        if (quiet == 0)
            perror("Bind() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("Executed Bind() successfully\n");

    char buffer[BUFFER_SIZE];

    socklen_t len_clientAddress = sizeof(struct sockaddr_un);

    FILE *outputFile = fopen("uds_dgram.txt", "wb");
    if (outputFile == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        close(serverFd);
        exit(1);
    }

    struct timeval start = {0}, end = {0};
    gettimeofday(&start, NULL);

    while (1)
    {
        int bytes_received = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverAddress, &len_clientAddress);
        if (bytes_received == -1)
        {
            if (quiet == 0)
                perror("recvfrom() failed\n");
            close(serverFd);
            fclose(outputFile);
            exit(-1);
        }
        else if (bytes_received == 0)
            break;
        else
        {
            if (strstr(buffer, "exit\n") != NULL)
                break;
            else
                fwrite(buffer, sizeof(char), bytes_received, outputFile);
        }
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("uds_dgram, %ld\n", total_time);

    fclose(outputFile);
    close(serverFd);
    unlink(SOCKET_PATH);
}

/**
 * Performs UNIX domain socket stream server operation.
 * @param quiet A flag indicating whether to display verbose output or not.
 */
void s_uds_stream(int quiet)
{
    int serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket\n");
        exit(-1);
    }
    else if (quiet == 0)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 0)
            perror("setsockopt() failed\n");
        exit(-1);
    }

    struct sockaddr_un serverAddress = {0}, clientAddress = {0};
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);
    unlink(SOCKET_PATH);
    int bindResult = bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un));
    if (bindResult == -1)
    {
        if (quiet == 0)
            perror("bind() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("Executed bind() successfully\n");

    int listenResult = listen(serverFd, 1);
    if (listenResult == -1)
    {
        if (quiet == 0)
            perror("listen() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("Waiting for incoming uds-connections...\n");

    socklen_t len_clientAddress = sizeof(struct sockaddr_un);

    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 0)
            perror("accept() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("A new client connection accepted\n");

    FILE *outputFile = fopen("uds_stream.txt", "w");
    if (outputFile == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        close(clientFd);
        close(serverFd);
        exit(1);
    }

    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read;

    //calc time
    struct timeval start = {0}, end = {0};
    gettimeofday(&start, NULL);

    while ((bytes_read = read(clientFd, buffer, sizeof(buffer))) > 0)
    {
        fwrite(buffer, sizeof(char), bytes_read, outputFile);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("uds_stream, %ld\n", total_time);

    fclose(outputFile);
    close(clientFd);
    close(serverFd);
}

/**
 * Performs memory-mapped file I/O using UDP.
 * @param serverPort The port number to bind the server socket.
 * @param quiet A flag indicating whether to display verbose output or not.
 */
void s_mmap(char *filename, int quiet)
{

    sleep(1);
    int shm_fd = shm_open(filename, O_RDONLY, 0666);
    if (shm_fd < 0)
    {
        perror("Shared memory open error");
        exit(1);
    }

    struct stat st;
    if (fstat(shm_fd, &st) < 0)
    {
        perror("Shared memory stat error");
        close(shm_fd);
        exit(1);
    }

    size_t size = st.st_size;

    void *addr = mmap(NULL, size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED)
    {
        if (quiet == 0)
            perror("mmap() failed\n");
        close(shm_fd);
        exit(1);
    }

    int fd = open("mmap.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
    {
        if (quiet == 0)
            perror("open() failed");
        munmap(addr, size);
        close(shm_fd);
        exit(1);
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    write(fd, addr, size);

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("mmap, %ld\n", total_time);

    munmap(addr, size);
    close(fd);
    close(shm_fd);
}

/**
 * Performs pipe-based communication.
 * @param quiet A flag indicating whether to display verbose output or not.
 */
void s_pipe(char* filename, int quiet)
{

    FILE *fp = fopen("pipe.txt", "w");
    if(fp == NULL){
        perror("fopen() failed\n");
        exit(1);
    }

    int fdFIFO;
    sleep(1);
    // Open the FIFO for reading
    if ((fdFIFO = open(filename, O_RDONLY)) == -1)
    {
        perror("open() failed");
        fclose(fp);
        exit(1);
    }

    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytesRead;

    struct timeval start,end;
    gettimeofday(&start, NULL);

    // Read the contents of the FIFO and print them to stdout
    while ((bytesRead = read(fdFIFO, buffer, BUFFER_SIZE)) > 0)
    {
        fwrite(buffer, 1, bytesRead, fp);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("pipe, %ld\n", total_time);


    // Close the FIFO
    close(fdFIFO);

    // Close the file.
    fclose(fp);

    // Remove the FIFO
    unlink(filename);

}

/**
 * Server function for handling incoming connections and routing them based on the received messages.
 * @param serverPort The port number to bind the server socket.
 * @param quiet A flag indicating whether to display verbose output or not.
 */
void serverB(int serverPort, int quiet)
{

    // Create socket
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket\n");
        exit(-1);
    }
    else if (quiet == 0)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 0)
            perror("setsockopt() failed\n");
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
        if (quiet == 0)
            perror("bind failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("Executed bind() successfully\n");

    // Listen for incoming connections
    int listenResult = listen(serverFd, 1);
    if (listenResult == -1)
    {
        if (quiet == 0)
            perror("listen() failed\n");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("Waiting for incoming connections...\n");

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 0)
            perror("listen failed\n");
        close(clientFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("A new client connection accepted\n");

    char buffer[2][4096];

    // Receive message1 from the client
    ssize_t num_bytes = recv(clientFd, buffer[0], sizeof(buffer[0]) - 1, 0);
    if (num_bytes == -1)
    {
        if (quiet == 0)
            perror("recv failed\n");
        close(clientFd);
        exit(-1);
    }

    buffer[0][num_bytes] = '\0';
    if (quiet == 0)
        printf("Received message1: %s\n", buffer[0]);

    // Receive message2 from the client
    num_bytes = recv(clientFd, buffer[1], sizeof(buffer[1]) - 1, 0);
    if (num_bytes == -1)
    {
        if (quiet == 0)
            perror("recv failed\n");
        close(clientFd);
        exit(-1);
    }

    buffer[1][num_bytes] = '\0';

    if (quiet == 0)
        printf("Received message2: %s\n", buffer[1]);


    // getting the hash from client
    unsigned long clientHash;
    if (recv(clientFd, &clientHash, sizeof(clientHash), 0) < 0)
    {
        perror("recv() failed");
        exit(1);
    }

    //need to be here(before the socket close)
    if(!strcmp(buffer[0], "pipe")){
        
        sleep(1);

        // Receive message2 from the client
        num_bytes = recv(clientFd, buffer[1], sizeof(buffer[1]) - 1, 0);
        if (num_bytes == -1)
        {
            if (quiet == 0)
                perror("recv failed\n");
            close(clientFd);
            exit(-1);
        }

        buffer[1][num_bytes] = '\0';

        //printf("buffer is: %s" ,buffer[1]);

        s_pipe(buffer[1],quiet);
    }

    // Close the connection
    close(clientFd);
    close(serverFd);

    if (!strcmp(buffer[0], "ipv4") && !strcmp(buffer[1], "tcp"))
        s_ipv4_tcp(serverPort, quiet);
    else if (!strcmp(buffer[0], "ipv4") && !strcmp(buffer[1], "udp"))
        s_ipv4_udp(serverPort, quiet);
    else if (!strcmp(buffer[0], "ipv6") && !strcmp(buffer[1], "tcp"))
        s_ipv6_tcp(serverPort, quiet);
    else if (!strcmp(buffer[0], "ipv6") && !strcmp(buffer[1], "udp"))
        s_ipv6_udp(serverPort, quiet);

    else if (!strcmp(buffer[0], "mmap") && !strcmp(buffer[1], "filename"))
        s_mmap(buffer[1], quiet);
    
    else if (!strcmp(buffer[0], "uds") && !strcmp(buffer[1], "dgram"))
        s_uds_dgram(quiet);

    else if (!strcmp(buffer[0], "uds") && !strcmp(buffer[1], "stream"))
        s_uds_stream(quiet);
}