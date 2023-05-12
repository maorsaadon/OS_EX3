#include "headers.h"

void s_ipv4_tcp(int serverPort, int quiet)
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

    // socklen_t len_clientAddress = {0};
    socklen_t len_clientAddress = sizeof(clientAddress);
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

    // char buffer[1024] = {0};
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

    // Receive start time from the client socket
    struct timeval start = {0}, end = {0};
    long buffer[2] = {0};
    int bytes_read = recv(clientFd, &buffer[0], sizeof(buffer[0]), 0);
    if (bytes_read == -1)
    {
        perror("read failed");
        close(clientFd);
        exit(-1);
    }

    bytes_read = recv(clientFd, &buffer[1], sizeof(buffer[1]), 0);
    if (bytes_read == -1)
    {
        perror("read failed");
        close(clientFd);
        exit(-1);
    }

    start.tv_sec = buffer[0];
    start.tv_usec = buffer[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        fputc(c, output_file);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - buffer[0];
    long microseconds = end.tv_usec - buffer[1];
    long total_time = seconds * 1000000 + microseconds;

    printf("{ipv4 tcp} Total time is : %ld\n", total_time);

    fclose(fp);
    close(clientFd);
    close(serverFd);
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

    // unsigned long long checksum = 0;
    char buffer[4096] = {0};
    FILE *fp;

    fp = fdopen(clientFd, "r");
    if (fp == NULL)
    {
        if (quiet == 1)
            printf("fdopen failed with error code : %d", errno);
        exit(1);
    }

    // Receive start time from the client socket
    struct timeval start = {0}, end = {0};
    long buffer_time[2] = {0};
    int bytes_read = recv(clientFd, &buffer_time[0], sizeof(buffer[0]), 0);
    if (bytes_read == -1)
    {
        perror("read failed");
        close(clientFd);
        exit(-1);
    }

    bytes_read = recv(clientFd, &buffer_time[1], sizeof(buffer[1]), 0);
    if (bytes_read == -1)
    {
        perror("read failed");
        close(clientFd);
        exit(-1);
    }

    start.tv_sec = buffer_time[0];
    start.tv_usec = buffer_time[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    size_t nbytes;
    while (1)
    {
        nbytes = fread(buffer, sizeof(char), sizeof(buffer), fp);
        if (nbytes >= 0)
        {
            break;
        }
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("{ipv6 tcp}  Total time is :  %ld\n", total_time);

    fclose(fp);
    close(clientFd);
    close(serverFd);
}

// times isnt seem correct
void s_ipv4_udp(int serverPort, int quiet)
{
    // Create socket
    int serverFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 1)
            perror("Could not create socket");
        exit(-1);
    }
    else if (quiet == 1)
        printf("New socket opened\n");

    int enableReuse = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        if (quiet == 1)
            perror("setsockopt() failed");
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress, clientAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = serverPort; // you've converted it before the function

    // Bind socket to address and port
    if (bind(serverFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        if (quiet == 1)
            perror("Bind failed");
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 1)
        printf("executed Bind() successfully\n");

    FILE *fp = fopen("ipv4_udp.txt", "w");
    if (fp == NULL)
    {
        if (quiet == 1)
            perror("fopen failed");
        exit(1);
    }

    long buffer_time_start[2] = {0};
    struct timeval start = {0}, end = {0};

    socklen_t len_clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    len_clientAddress = sizeof(clientAddress);

    // Receive start time from the client socket
    if (recvfrom(serverFd, &buffer_time_start[0], sizeof(buffer_time_start[0]), 0, (struct sockaddr *)&clientAddress, &len_clientAddress) == -1)
    {
        if (quiet == 1)
            perror("recv failed");
        close(serverFd);
        exit(-1);
    }

    if (recvfrom(serverFd, &buffer_time_start[1], sizeof(buffer_time_start[1]), 0, (struct sockaddr *)&clientAddress, &len_clientAddress) == -1)
    {
        if (quiet == 1)
            perror("recv failed");
        close(serverFd);
        exit(-1);
    }

    start.tv_sec = buffer_time_start[0];
    start.tv_usec = buffer_time_start[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = serverFd;
    fds[0].events = POLLIN;

    char buffer[BUFFER_SIZE] = {0};
    int total_bytes_received = 0;
    // Inside the while loop
    while (1)
    {
        int pull = poll(fds, MAX_EVENTS, -1);
        if (pull == -1)
        {
            perror("Pull failed");
            exit(-1);
        }

        if (fds[0].revents & POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            long long bytes_received = recvfrom(serverFd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 1)
                    perror("recvfrom() failed");
                close(serverFd);
                fclose(fp);
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
                    total_bytes_received += bytes_received;
                    fwrite(buffer, sizeof(char), bytes_received, fp);
                }
            }
        }
    }

    gettimeofday(&end, NULL);

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    printf("end time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    long total_time = seconds * 1000000 + microseconds;

    printf("{ipv4 udp} Total time is : %ld\n", total_time);

    close(serverFd);
    fclose(fp);
}

// need to fix that when client send all data server just stop and not quit the while loop
void s_ipv6_udp(int serverPort, int quiet)
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

    char buffer[4096] = {0};
    // socklen_t len_clientAddress = {0};
    // len_clientAddress = sizeof(struct sockaddr_in6);

    FILE *fp = fopen("ipv6_udp.txt", "w");
    if (fp == NULL)
    {
        if (quiet == 1)
            printf("fopen failed with error code : %d", errno);
        exit(1);
    }

    // Receive start time from the client socket
    struct timeval start = {0}, end = {0};
    long buffer_time[2] = {0};
    socklen_t len_clientAddress = sizeof(clientAddress);

    int bytes_received = recvfrom(serverFd, &buffer_time, sizeof(buffer_time), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (bytes_received == -1)
    {
        perror("read failed");
        close(serverFd);
        exit(-1);
    }

    start.tv_sec = buffer_time[0];
    start.tv_usec = buffer_time[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    int running = 1;
    while (running)
    {

        int pull = poll(fds, 2, -1);
        if (pull == -1)
        {
            if (quiet == 1)
                printf("Pull failed with error code : %d", errno);
            exit(-1);
        }

        if (fds[0].revents & POLLIN)
        {

            bytes_received = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 1)
                    printf("recv failed with error code : %d", errno);
                exit(-1);
            }

            if (bytes_received == 0 && (recvfrom(serverFd, buffer, sizeof(buffer), MSG_PEEK, (struct sockaddr *)&clientAddress, &len_clientAddress) == 0))
            {
                printf("client disconnected\n");
                running = 0;
            }

            fwrite(buffer, sizeof(char), bytes_received, fp);

            if (ftell(fp) >= 100000000)
            {
                running = 0;
            }

            if (!strcmp(buffer, "exit\n"))
            {
                running = 0;
            }

            // if(bytes_received < 8){
            //     running = 0;
            // }
        }
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("{ipv6 udp} Total time is : %ld\n", total_time);

    fclose(fp);
    close(serverFd);
}

// need to fix that when client send all data server just stop and not quit the while loop
void s_uds_dgram(int quiet)
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
    // socklen_t len_clientAddress = {0};
    // len_clientAddress = sizeof(struct sockaddr_un);

    // Receive start time from the client socket
    struct timeval start = {0}, end = {0};
    long buffer_time[2] = {0};
    socklen_t len_clientAddress = sizeof(clientAddress);

    int bytes_received = recvfrom(serverFd, &buffer_time, sizeof(buffer_time), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (bytes_received == -1)
    {
        perror("read failed");
        close(serverFd);
        exit(-1);
    }

    start.tv_sec = buffer_time[0];
    start.tv_usec = buffer_time[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

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

            bytes_received = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 1)
                    printf("recv failed with error code : %d", errno);
                exit(-1);
            }

            if (!strcmp(buffer, "exit\n"))
                break;
        }
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("{uds dgram} Total time is : %ld", total_time);

    close(serverFd);
    unlink(SOCKET_PATH);
}

// need to add a new file and copy there all the data received from server
void s_uds_stream(int quiet)
{
    int serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
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
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);
    unlink(SOCKET_PATH);
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
    len_clientAddress = sizeof(struct sockaddr_un);
    int clientFd = 0;
    // char buffer[1024] = {0};
    int timeout = 5000;

    struct pollfd fds[1];
    fds[0].fd = serverFd;
    fds[0].events = POLLIN;

    struct timeval start = {0}, end = {0};

    while (1)
    {
        int pull = poll(fds, 1, timeout);
        if (pull == -1)
        {
            if (quiet == 1)
                printf("Pull failed with error code : %d", errno);
            exit(-1);
        }
        else if (pull == 0)
            continue;

        if (fds[0].revents & POLLIN)
        {
            clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (clientFd == -1)
            {
                if (quiet == 1)
                    printf("listen failed with error code : %d", errno);
                close(clientFd);
                exit(-1);
            }
            else if (quiet == 1)
                printf("A new client connection accepted\n");

            FILE *fp = fdopen(clientFd, "r");
            if (fp == NULL)
            {
                if (quiet == 1)
                    printf("fdopen failed with error code : %d", errno);
                exit(1);
            }

            // Receive start time from the client socket
            long buffer[2] = {0};
            int bytes_read = recv(clientFd, &buffer[0], sizeof(buffer[0]), 0);
            if (bytes_read == -1)
            {
                perror("read failed");
                close(clientFd);
                exit(-1);
            }

            bytes_read = recv(clientFd, &buffer[1], sizeof(buffer[1]), 0);
            if (bytes_read == -1)
            {
                perror("read failed");
                close(clientFd);
                exit(-1);
            }

            start.tv_sec = buffer[0];
            start.tv_usec = buffer[1];

            printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

            fclose(fp);
            break;
        }
        close(clientFd);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("{uds dgram} Total time is : %ld\n", total_time);

    close(serverFd);
}

// need to create a new file and copy all the data to there
void s_mmap(int serverPort, int quiet)
{

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

    struct sockaddr_in serverAddress = {0}, clientAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = serverPort;
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

    char buffer[BUFFER_SIZE] = {0};
    int timeout = 5000;

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);

    struct pollfd fds[1];
    fds[0].fd = serverFd;
    fds[0].events = POLLIN;

    // Receive start time from the client socket
    struct timeval start = {0}, end = {0};
    long buffer_time[2] = {0};

    int bytes_received = recvfrom(serverFd, &buffer_time, sizeof(buffer_time), 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (bytes_received == -1)
    {
        perror("read failed");
        close(serverFd);
        exit(-1);
    }

    start.tv_sec = buffer_time[0];
    start.tv_usec = buffer_time[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    while (1)
    {
        int pull = poll(fds, 1, timeout);
        if (pull == -1)
        {
            if (quiet == 1)
                printf("Pull failed with error code : %d", errno);
            exit(-1);
        }
        else if (pull == 0)
            break;
        else
        {
            if (fds[0].revents & POLLIN)
            {

                int bytes_received = recvfrom(serverFd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
                if (bytes_received == -1)
                {
                    if (quiet == 1)
                        printf("recv failed with error code : %d", errno);
                    exit(-1);
                }
            }
        }
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("{ipv4 udp} Total time is : %ld\n", total_time);

    close(serverFd);
}

// need to create a new file and copy all the data to there
void s_pipe(int quiet)
{

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    mkfifo(FIFO_NAME, 0666);
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1)
    {
        if (quiet == 1)
            printf("pipe failed with error code : %d", errno);
        exit(-1);
    }
    FILE *fp = fopen("received_file.txt", "w");
    if (fp == NULL)
    {
        if (quiet == 1)
            printf("fdopen failed with error code : %d", errno);
        exit(1);
    }

    // Receive start time from the named pipe
    struct timeval start = {0}, end = {0};
    long buffer_time[2] = {0};

    bytes_read = read(fd, &buffer_time[0], sizeof(buffer_time[0]));
    if (bytes_read == -1)
    {
        perror("read failed");
        close(fd);
        exit(-1);
    }

    bytes_read = read(fd, &buffer_time[1], sizeof(buffer_time[1]));
    if (bytes_read == -1)
    {
        perror("read failed");
        close(fd);
        exit(-1);
    }

    start.tv_sec = buffer_time[0];
    start.tv_usec = buffer_time[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0)
    {
        fwrite(buffer, 1, bytes_read, fp);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("{pipe} Total time is : %ld\n", total_time);

    fclose(fp);
    close(fd);
    unlink(FIFO_NAME);
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

    char buffer[2][4096];

    // Receive message1 from the client
    ssize_t num_bytes = recv(clientFd, buffer[0], sizeof(buffer[0]) - 1, 0);
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

    if (!strcmp(buffer[0], "ipv4") && !strcmp(buffer[1], "tcp"))
        s_ipv4_tcp(serverPort, quiet);

    else if (!strcmp(buffer[0], "ipv4") && !strcmp(buffer[1], "udp"))
        s_ipv4_udp(serverPort, quiet);

    else if (!strcmp(buffer[0], "ipv6") && !strcmp(buffer[1], "tcp"))
        s_ipv6_tcp(serverPort, quiet);

    else if (!strcmp(buffer[0], "ipv6") && !strcmp(buffer[1], "udp"))
        s_ipv6_udp(serverPort, quiet);

    else if (!strcmp(buffer[0], "mmap") && !strcmp(buffer[1], "filename"))
    {
        s_mmap(serverPort, quiet);
    }

    else if (!strcmp(buffer[0], "pipe") && !strcmp(buffer[1], "filename"))
    {
        s_pipe(quiet);
    }

    else if (!strcmp(buffer[0], "uds") && !strcmp(buffer[1], "dgram"))
        s_uds_dgram(quiet);

    else if (!strcmp(buffer[0], "uds") && !strcmp(buffer[1], "stream"))
    {
        s_uds_stream(quiet);
    }
}