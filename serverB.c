#include "headers.h"

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

    FILE *fp = fdopen(clientFd, "r");
    if (fp == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    FILE *of = fopen("ipv4_tcp.txt", "w");
    if (of == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    // Receive start time from the client socket
    struct timeval start = {0}, end = {0};
    long buffer[2] = {0};
    int bytes_read = recv(clientFd, &buffer[0], sizeof(buffer[0]), 0);
    if (bytes_read == -1)
    {
        if (quiet == 0)
            perror("recv() failed\n");
        close(clientFd);
        exit(-1);
    }

    bytes_read = recv(clientFd, &buffer[1], sizeof(buffer[1]), 0);
    if (bytes_read == -1)
    {
        if (quiet == 0)
            perror("recv() failed\n");
        close(clientFd);
        exit(-1);
    }

    start.tv_sec = buffer[0];
    start.tv_usec = buffer[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        fputc(c, of);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - buffer[0];
    long microseconds = end.tv_usec - buffer[1];
    long total_time = seconds * 1000000 + microseconds;

    printf("{ipv4 tcp} Total time is : %ld\n", total_time);

    fclose(of);
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
    struct sockaddr_in6 serverAddress = {0}, clientAddress = {0};
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port = serverPort;
    serverAddress.sin6_addr = in6addr_any;

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

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(struct sockaddr_in6);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 0)
            perror("listen() failed\n");
        close(clientFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("A new client connection accepted\n");

    char ip_str[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &clientAddress.sin6_addr, ip_str, INET6_ADDRSTRLEN);

    // unsigned long long checksum = 0;
    char buffer[BUFFER_SIZE] = {0};

    FILE *fp = fdopen(clientFd, "r");
    if (fp == NULL)
    {
        if (quiet == 0)
            perror("fdopen() failed\n");
        exit(1);
    }

    FILE *of = fopen("ipv6_tcp.txt", "w");
    if (of == NULL)
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

        fclose(of);
        fclose(fp);
        close(clientFd);
        exit(-1);
    }

    // Receive start time from the client socket
    struct timeval start = {0}, end = {0};
    long buffer_time[2] = {0};
    bytes_read = recv(clientFd, &buffer_time[0], sizeof(buffer[0]), 0);
    if (bytes_read == -1)
    {
        if (quiet == 0)
            perror("recv() failed\n");
        fclose(fp);
        close(clientFd);
        exit(-1);
    }

    bytes_read = recv(clientFd, &buffer_time[1], sizeof(buffer[1]), 0);
    if (bytes_read == -1)
    {
        if (quiet == 0)
            perror("recv() failed\n");
        fclose(fp);
        close(clientFd);
        exit(-1);
    }

    start.tv_sec = buffer_time[0];
    start.tv_usec = buffer_time[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    size_t total_bytes = 0;
    while (total_bytes < file_size)
    {
        size_t nbytes = recv(clientFd, buffer, sizeof(buffer), 0);
        if (nbytes <= 0)
        {
            if (quiet == 0)
                perror("recv() failed\n");
            fclose(of);
            fclose(fp);
            close(clientFd);
            exit(-1);
        }

        total_bytes += nbytes;
        fwrite(buffer, sizeof(char), nbytes, of);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("{ipv6 tcp}  Total time is :  %ld\n", total_time);

    fclose(of);
    fclose(fp);
    close(clientFd);
    exit(-1);
}

void s_ipv4_udp(int serverPort, int quiet)
{
    // Create socket
    int serverFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket");
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

    FILE *of = fopen("ipv4_udp.txt", "w");
    if (of == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    long buffer_time_start[2] = {0};
    struct timeval start = {0}, end = {0};

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);

    // Receive start time from the client socket
    if (recvfrom(serverFd, &buffer_time_start[0], sizeof(buffer_time_start[0]), 0, (struct sockaddr *)&clientAddress, &len_clientAddress) == -1)
    {
        if (quiet == 0)
            perror("recvfrom() failed\n");
        fclose(of);
        close(serverFd);
        exit(-1);
    }

    if (recvfrom(serverFd, &buffer_time_start[1], sizeof(buffer_time_start[1]), 0, (struct sockaddr *)&clientAddress, &len_clientAddress) == -1)
    {
        if (quiet == 0)
            perror("recvfrom() failed\n");
        fclose(of);
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
            if (quiet == 0)
                perror("Poll() failed\n");
            exit(-1);
        }

        if (fds[0].revents & POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            long long bytes_received = recvfrom(serverFd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 0)
                    perror("recvfrom() failed\n");
                close(serverFd);
                fclose(of);
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
                    fwrite(buffer, sizeof(char), bytes_received, of);
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
    fclose(of);
}

void s_ipv6_udp(int serverPort, int quiet)
{

    // Create socket
    int serverFd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (serverFd == -1)
    {
        if (quiet == 0)
            perror("Could not create socket");
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

    FILE *of = fopen("ipv6_udp.txt", "w");
    if (of == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        exit(1);
    }

    long buffer_time[2] = {0};
    struct timeval start = {0}, end = {0};

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);

    // Receive start time from the client socket
    if (recvfrom(serverFd, &buffer_time[0], sizeof(buffer_time[0]), 0, (struct sockaddr *)&clientAddress, &len_clientAddress) == -1)
    {
        if (quiet == 0)
            perror("recvfrom() failed\n");
        fclose(of);
        close(serverFd);
        exit(-1);
    }

    if (recvfrom(serverFd, &buffer_time[1], sizeof(buffer_time[1]), 0, (struct sockaddr *)&clientAddress, &len_clientAddress) == -1)
    {
        if (quiet == 0)
            perror("recvfrom() failed\n");
        fclose(of);
        close(serverFd);
        exit(-1);
    }

    start.tv_sec = buffer_time[0];
    start.tv_usec = buffer_time[1];

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
            if (quiet == 0)
                perror("Poll() failed\n");
            exit(-1);
        }

        if (fds[0].revents & POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            long long bytes_received = recvfrom(serverFd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&clientAddress, &len_clientAddress);
            if (bytes_received == -1)
            {
                if (quiet == 0)
                    perror("recvfrom() failed\n");
                close(serverFd);
                fclose(of);
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
                    fwrite(buffer, sizeof(char), bytes_received, of);
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
    fclose(of);
}

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

    char buffer[1024];
    struct timeval start = {0}, end = {0};
    long buffer_time[2] = {0};
    socklen_t len_clientAddress = sizeof(struct sockaddr_un);

    int bytes_received = recvfrom(serverFd, &buffer_time, sizeof(buffer_time), 0, (struct sockaddr *)&serverAddress, &len_clientAddress);
    if (bytes_received == -1)
    {
        if (quiet == 0)
            perror("recvfrom() failed\n");
        close(serverFd);
        exit(-1);
    }

    start.tv_sec = buffer_time[0];
    start.tv_usec = buffer_time[1];

    printf("start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

    FILE *of = fopen("uds_dgram.txt", "wb");
    if (of == NULL)
    {
        if (quiet == 0)
            perror("fopen() failed\n");
        close(serverFd);
        exit(1);
    }

    while (1)
    {
        bytes_received = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverAddress, &len_clientAddress);
        if (bytes_received == -1)
        {
            if (quiet == 0)
                perror("recvfrom() failed\n");
            fclose(of);
            close(serverFd);
            exit(-1);
        }

        if (!strcmp(buffer, "exit\n"))
            break;

        fwrite(buffer, sizeof(char), bytes_received, of);
    }

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    long total_time = seconds * 1000000 + microseconds;

    printf("{uds dgram} Total time is : %ld\n", total_time);

    fclose(of);
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
        if (quiet == 0)
            printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else if (quiet == 0)
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        if (quiet == 0)
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
        if (quiet == 0)
            printf("Bind failed with error code : %d", errno);
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
            printf("listen() failed with error code : %d", errno);
        close(serverFd);
        exit(-1);
    }
    else if (quiet == 0)
        printf("Waiting for incoming TCP-connections...\n");

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientFd == -1)
    {
        if (quiet == 0)
            printf("listen failed with error code : %d", errno);
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
            printf("recv failed with error code : %d", errno);
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
            printf("recv failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }

    buffer[1][num_bytes] = '\0';

    if (quiet == 0)
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