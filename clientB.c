
#include "headers.h"

void generate_data_to_file()
{
    // Write the data to a file
    FILE *fp = fopen("data.txt", "w+");
    if (fp == NULL)
    {
        printf("Error: Unable to open file for writing!\n");
        exit(1);
    }

    int counter = 0;
    while (counter <= 100 * 1024 * 1024)
    {
        // Seed the random number generator
        srand(time(NULL));
        
        // Generate a random string of length 1024
        char data[1025] = {0}; // Initialize with null characters
        for (int i = 0; i < 1024; i++)
        {
            data[i] = (char) (rand() % 26 + 'a'); // Generate a random lowercase letter
        }

        // Fill the data with random strings
        fprintf(fp, "%s", data);
        counter += 1024;
    }

    fclose(fp);

    return;
}

void ipv4_tcp_client(char *serverIp, int serverPort)
{
    // Create socket
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd == -1)
    {
        printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverIp);
    serverAddress.sin_port = serverPort;

    // Connect to server
    int connectResult = connect(clientFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }
    else
        printf("connected to receiver\n");

    // Open input file for reading
    int file_fd = open("data.txt", O_RDONLY);
    if (file_fd == -1)
    {
        printf("fopen failed with error code : %d", errno);
        close(clientFd);
        exit(1);
    }

    // calc time
    //  Get the start time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Create a buffer to hold the start time
    char buffer[BUFFER_SIZE] = {0};
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (send(clientFd, start_time_buffer, sizeof(start_time_buffer), 0) == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(clientFd);
        close(file_fd);
        exit(-1);
    }

    if (send(clientFd, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0) == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(clientFd);
        close(file_fd);
        exit(-1);
    }

    int bytes_sent;
    while ((bytes_sent = read(file_fd, buffer, BUFFER_SIZE)) > 0)
    {
        if (write(clientFd, buffer, bytes_sent) == -1)
        {
            printf("write() failed with error code : %d", errno);
            close(clientFd);
            close(file_fd);
            exit(-1);
        }
    }

    if (bytes_sent == -1)
    {
        printf("read() failed with error code : %d", errno);
        close(clientFd);
        close(file_fd);
        exit(-1);
    }

    close(file_fd);
    close(clientFd);
}

// problem with the write/send to server
void ipv6_tcp_client(char *serverIp, int serverPort)
{

    // Create socket
    int clientFd = socket(AF_INET6, SOCK_STREAM, 0);
    if (clientFd == -1)
    {
        printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    struct sockaddr_in6 serverAddress = {0};
    serverAddress.sin6_family = AF_INET6;
    inet_pton(AF_INET6, serverIp, &serverAddress.sin6_addr);
    serverAddress.sin6_port = serverPort;

    // Connect to server
    int connectResult = connect(clientFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }
    else
        printf("connected to receiver\n");

    FILE *of = fopen("data.txt", "r");
    if (of == NULL)
    {
        printf("fopen failed with error code : %d", errno);
        exit(1);
    }

    // calc time
    //  Get the start time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Create a buffer to hold the start time
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (send(clientFd, start_time_buffer, sizeof(start_time_buffer), 0) == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    if (send(clientFd, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0) == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    char buffer[BUFFER_SIZE] = {0};
    size_t nbytes, total_bytes = 0;
    while ((nbytes = fread(buffer, sizeof(char), sizeof(buffer), of)) > 0)
    {
        total_bytes += nbytes;
        if (send(clientFd, buffer, nbytes, 0) == -1)
        {
            printf("send() failed with error code : %d", errno);
            close(clientFd);
            fclose(of);
            exit(-1);
        }
    }

    fclose(of);
    close(clientFd);
}

void ipv4_udp_client(char *serverIp, int serverPort)
{
    // Create socket
    int clientFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientFd == -1)
    {
        perror("Could not create socket");
        exit(-1);
    }
    else
        printf("New socket opened\n");

    int enableReuse = 1;
    if (setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        perror("setsockopt() failed");
        exit(-1);
    }

    struct sockaddr_in serverAdress = {0};
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = serverPort; // you've converted it before the function
    serverAdress.sin_addr.s_addr = inet_addr(serverIp);

    FILE *of = fopen("data.txt", "r");
    if (of == NULL)
    {
        perror("fopen failed");
        exit(1);
    }

    char buffer[BUFFER_SIZE] = {0};
    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = clientFd;
    fds[0].events = POLLOUT;
    fds[1].fd = fileno(of);
    fds[1].events = POLLIN;
    int nfds = 2;

    // Get the start time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Create a buffer to hold the start time
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (sendto(clientFd, &start_time_buffer[0], sizeof(start_time_buffer[0]), 0, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) == -1)
    {
        perror("send() failed");
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    if (sendto(clientFd, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) == -1)
    {
        perror("send() failed");
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    while (1)
    {
        int pull = poll(fds, nfds, -1);
        if (pull == -1)
        {
            perror("Pull failed");
            exit(-1);
        }
        else if (pull == 0)
            continue;

        if (fds[0].revents & POLLOUT)
        {
            if (fgets(buffer, sizeof(buffer), of) != NULL)
            {
                if (sendto(clientFd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAdress, sizeof(struct sockaddr)) == -1)
                {
                    perror("sendto() failed");
                    close(clientFd);
                    fclose(of);
                    exit(-1);
                }
            }
            else
            {
                close(fds[1].fd);
                nfds--;
            }
        }
        if (nfds == 0)
        {
            break;
        }
    }

    char buffer_exit[6] = "exit\n";
    if (sendto(clientFd, buffer_exit, strlen(buffer_exit), 0, (struct sockaddr *)&serverAdress, sizeof(struct sockaddr)) == -1)
    {
        perror("send() failed");
        close(clientFd);
        fclose(of);
        exit(-1);
    }
    close(clientFd);
    fclose(of);
}

// need to check why its take so long to finish the main and quit
void ipv6_udp_client(char *serverIp, int serverPort)
{
    // Create socket
    int clientFd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (clientFd == -1)
    {
        printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    struct sockaddr_in6 serverAddress;
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port = serverPort;
    inet_pton(AF_INET6, serverIp, &(serverAddress.sin6_addr));
    FILE *of = fopen("data.txt", "r");
    if (of == NULL)
    {
        printf("fopen failed with error code : %d", errno);
        exit(1);
    }
    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = clientFd;
    fds[0].events = POLLOUT;

    // Get the start time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Create a buffer to hold the start time
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (sendto(clientFd, &start_time_buffer[0], sizeof(start_time_buffer[0]), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    if (sendto(clientFd, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("send() failed with error code : %d", errno);
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    char buffer[BUFFER_SIZE] = {0};
    while (1)
    {
        int pull = poll(fds, MAX_EVENTS, -1);
        if (pull == -1)
        {
            printf("Pull failed with error code : %d", errno);
            exit(-1);
        }
        else if (pull == 0)
            continue;

        if (fds[0].revents & POLLOUT)
        {

            memset(buffer, 0, sizeof(buffer));
            if (fgets(buffer, sizeof(buffer), of) != NULL)
            {
                if (sendto(clientFd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in6)) == -1)
                {
                    printf("sendto() failed with error code : %d", errno);
                    close(clientFd);
                    fclose(of);
                    exit(-1);
                }
            }
            else
            {
                strcpy(buffer, "exit\n");
                if (sendto(clientFd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in6)) == -1)
                {
                    printf("sendto() failed with error code : %d", errno);
                    close(clientFd);
                    fclose(of);
                    exit(-1);
                }

                break;
            }
        }
    }

    fclose(of);
    close(clientFd);
}

void uds_dgram_client(char *serverIp, int serverPort)
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

    // Get the start time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Create a buffer to hold the start time
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (sendto(client_socket, &start_time_buffer[0], sizeof(start_time_buffer[0]), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("sendto");
        exit(1);
    }

    if (sendto(client_socket, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("sendto");
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
}

void uds_stream_client()
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

    // calc time
    //  Get the start time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Create a buffer to hold the start time
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (send(client_socket, start_time_buffer, sizeof(start_time_buffer), 0) == -1)
    {
        perror("write failed");
        close(client_socket);
        fclose(fp);
        exit(-1);
    }

    if (send(client_socket, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0) == -1)
    {
        perror("write failed");
        close(client_socket);
        fclose(fp);
        exit(-1);
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
}

void mmap_client(int serverPort)
{

    struct sockaddr_in serv_addr;
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
    serv_addr.sin_port = serverPort;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Get the start time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Create a buffer to hold the start time
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (sendto(sockfd, &start_time_buffer[0], sizeof(start_time_buffer[0]), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("sendto");
        exit(1);
    }

    if (sendto(sockfd, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("sendto");
        exit(1);
    }

    size_t remaining = filestat.st_size;
    char *data_ptr = filedata;

    while (remaining > 0)
    {
        size_t send_size = remaining > 4096 ? 4096 : remaining;
        int n = sendto(sockfd, data_ptr, send_size, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (n < 0)
        {
            perror("sendto");
        }
        data_ptr += send_size;
        remaining -= send_size;
    }

    close(fd);
    close(sockfd);
}

void pipe_client()
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

    // Get the start time
    struct timeval start;
    gettimeofday(&start, NULL);

    // Create a buffer to hold the start time
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (write(fd, &start_time_buffer[0], sizeof(start_time_buffer[0])) == -1)
    {
        perror("write");
        exit(1);
    }

    if (write(fd, &start_time_buffer[1], sizeof(start_time_buffer[1])) == -1)
    {
        perror("write");
        exit(1);
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        write(fd, buffer, bytes_read);
    }
    fclose(file);
    close(fd);
}

void clientB(char *serverIp, int serverPort, char *type, char *param)
{
    // create file that 100mb size
    generate_data_to_file();

    // Create socket
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd == -1)
    {
        printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverIp);
    serverAddress.sin_port = serverPort;

    // Connect to server
    int connectResult = connect(clientFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }
    else
        printf("connected to receiver\n");

    printf("message1: %s\n", type);
    printf("message2: %s\n", param);

    // Send data to the server
    if (send(clientFd, type, strlen(type), 0) == -1)
    {
        printf("send() failed with error code : %d", errno);
        exit(-1);
    }
    sleep(1);
    if (send(clientFd, param, strlen(param), 0) == -1)
    {
        printf("send() failed with error code : %d", errno);
        exit(-1);
    }

    // Close the connection
    close(clientFd);

    sleep(1);

    if (!strcmp(type, "ipv4") && !strcmp(param, "tcp"))
        ipv4_tcp_client(serverIp, serverPort);

    else if (!strcmp(type, "ipv4") && !strcmp(param, "udp"))
    {
        ipv4_udp_client(serverIp, serverPort);
    }
    else if (!strcmp(type, "ipv6") && !strcmp(param, "tcp"))
        ipv6_tcp_client(serverIp, serverPort);

    else if (!strcmp(type, "ipv6") && !strcmp(param, "udp"))
        ipv6_udp_client(serverIp, serverPort);

    else if (!strcmp(type, "mmap") && !strcmp(param, "filename"))
        mmap_client(serverPort);

    else if (!strcmp(type, "pipe") && !strcmp(param, "filename"))
        pipe_client();

    else if (!strcmp(type, "uds") && !strcmp(param, "dgram"))
        uds_dgram_client(serverIp, serverPort);

    else if (!strcmp(type, "uds") && !strcmp(param, "stream"))
        uds_stream_client();
}
