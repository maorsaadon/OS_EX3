
#include "headers.h"

/**
 * Generates data and writes it to a file named "data.txt".
 * The generated data consists of random strings of length 1024 repeated multiple times.
 * Each string is written to the file using fprintf.
 */
void generate_data_to_file()
{
    FILE *fp = fopen("data.txt", "w+");
    if (fp == NULL)
    {
        printf("Error: Unable to open file for writing!\n");
        exit(1);
    }

    int counter = 0;
    while (counter <= 100 * 1024 * 1024)
    {
        srand(time(NULL));

        char data[1025] = {0}; 
        for (int i = 0; i < 1024; i++)
            data[i] = (char)(rand() % 26 + 'a'); 

        fprintf(fp, "%s", data);
        counter += 1024;
    }

    fclose(fp);

    return;
}

/**
 * Connects to a server using IPv4 and TCP protocol.
 * Reads data from a file named "data.txt" and sends it to the server.
 *
 * @param serverIp   The IP address of the server.
 * @param serverPort The port number of the server.
 */
void c_ipv4_tcp(char *serverIp, int serverPort)
{
    // Create socket
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd == -1)
    {
        perror("Could not create socket \n");
        exit(-1);
    }

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        perror("setsockopt() failed\n");
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
        perror("connect() failed\n");
        close(clientFd);
        exit(-1);
    }

    // Open input file for reading
    int file_fd = open("data.txt", O_RDONLY);
    if (file_fd == -1)
    {
        printf("fopen() failed\n");
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
    if (send(clientFd, &start_time_buffer[0], sizeof(start_time_buffer[0]), 0) == -1)
    {
        printf("send() failed\n");
        close(clientFd);
        close(file_fd);
        exit(-1);
    }

    if (send(clientFd, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0) == -1)
    {
        printf("send() failed\n");
        close(clientFd);
        close(file_fd);
        exit(-1);
    }

    int bytes_sent;
    while ((bytes_sent = read(file_fd, buffer, BUFFER_SIZE)) > 0)
    {
        if (write(clientFd, buffer, bytes_sent) == -1)
        {
            printf("write() failed\n");
            close(clientFd);
            close(file_fd);
            exit(-1);
        }
    }

    if (bytes_sent == -1)
    {
        printf("read() failed\n");
        close(clientFd);
        close(file_fd);
        exit(-1);
    }

    close(file_fd);
    close(clientFd);
}

/**
 * Connects to a server using IPv6 and TCP protocol.
 * Reads data from a file named "data.txt" and sends it to the server.
 * @param serverIp The IPv6 address of the server.
 * @param serverPort The port number of the server.
 */
void c_ipv6_tcp(char *serverIp, int serverPort)
{
    // Create socket
    int clientFd = socket(AF_INET6, SOCK_STREAM, 0);
    if (clientFd == -1)
    {
        perror("Could not create socket\n");
        exit(-1);
    }

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        perror("setsockopt() failed\n");
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
        perror("connect() failed\n");
        close(clientFd);
        exit(-1);
    }

    FILE *of = fopen("data.txt", "rb");
    if (of == NULL)
    {
        perror("fopen() failed\n");
        exit(-1);
    }

    // Send file size to the server
    fseek(of, 0, SEEK_END);
    long file_size = ftell(of);
    fseek(of, 0, SEEK_SET);

    if (send(clientFd, &file_size, sizeof(file_size), 0) == -1)
    {
        perror("send() failed\n");
        close(clientFd);
        fclose(of);
        exit(-1);
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
        perror("send() failed\n");
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    if (send(clientFd, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0) == -1)
    {
        perror("send() failed\n");
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    char buffer[BUFFER_SIZE];
    size_t nbytes = 0;
    while ((nbytes = fread(buffer, sizeof(char), sizeof(buffer), of)) > 0)
    {
        if (send(clientFd, buffer, nbytes, 0) == -1)
        {
            perror("send() failed\n");
            close(clientFd);
            fclose(of);
            exit(-1);
        }
    }

    fclose(of);
    close(clientFd);
}

/**
 * Connects to a server using IPv4 and UDP protocol.
 * Reads data from a file named "data.txt" and sends it to the server.
 * @param serverIp The IP address of the server.
 * @param serverPort The port number of the server.
*/
void c_ipv4_udp(char *serverIp, int serverPort)
{
    // Create socket
    int clientFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientFd == -1)
    {
        perror("Could not create socket\n");
        exit(-1);
    }

    int enableReuse = 1;
    if (setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        perror("setsockopt() failed\n");
        exit(-1);
    }

    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = serverPort; // you've converted it before the function
    serverAddress.sin_addr.s_addr = inet_addr(serverIp);

    FILE *of = fopen("data.txt", "r");
    if (of == NULL)
    {
        perror("fopen() failed\n");
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
    if (sendto(clientFd, &start_time_buffer[0], sizeof(start_time_buffer[0]), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("send() failed\n");
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    if (sendto(clientFd, &start_time_buffer[1], sizeof(start_time_buffer[1]), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("send() failed\n");
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    while (1)
    {
        int pull = poll(fds, nfds, -1);
        if (pull == -1)
        {
            perror("Pull failed\n");
            exit(-1);
        }
        else if (pull == 0)
            continue;

        if (fds[0].revents & POLLOUT)
        {
            if (fgets(buffer, sizeof(buffer), of) != NULL)
            {
                if (sendto(clientFd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr)) == -1)
                {
                    perror("sendto() failed\n");
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
    if (sendto(clientFd, buffer_exit, strlen(buffer_exit), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr)) == -1)
    {
        perror("send() failed\n");
        close(clientFd);
        fclose(of);
        exit(-1);
    }

    close(clientFd);
    fclose(of);
}

/**
 * Connects to a server using IPv6 and UDP protocol.
 * Reads data from a file named "data.txt" and sends it to the server.
 * @param serverIp The IPv6 address of the server.
 * @param serverPort The port number of the server.
*/
void c_ipv6_udp(char *serverIp, int serverPort)
{
    int clientFd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (clientFd == -1)
    {
        perror("Could not create socket\n");
        exit(-1);
    }

    int enableReuse = 1;
    if (setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
        perror("setsockopt() failed\n");
        exit(-1);
    }

    struct sockaddr_in6 serverAddress = {0};
    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_port = serverPort;
    inet_pton(AF_INET6, serverIp, &(serverAddress.sin6_addr));

    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        perror("fopen() failed\n");
        exit(1);
    }

    struct pollfd fds[MAX_EVENTS];
    fds[0].fd = clientFd;
    fds[0].events = POLLOUT;
    fds[1].fd = fileno(fp);
    fds[1].events = POLLIN;
    int nfds = 2;

    struct timeval start = {0};
    gettimeofday(&start, NULL);

    long buffer_time[2] = {0};
    buffer_time[0] = start.tv_sec;
    buffer_time[1] = start.tv_usec;

    if (sendto(clientFd, &buffer_time[0], sizeof(buffer_time[0]), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in6)) == -1)
    {
        perror("send() failed\n");
        close(clientFd);
        fclose(fp);
        exit(-1);
    }

    if (sendto(clientFd, &buffer_time[1], sizeof(buffer_time[1]), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in6)) == -1)
    {
        perror("send() failed\n");
        close(clientFd);
        fclose(fp);
        exit(-1);
    }

    char buffer[BUFFER_SIZE] = {0};
    while (1)
    {
        int pull = poll(fds, nfds, -1);
        if (pull == -1)
        {
            perror("Pull() failed\n");
            exit(-1);
        }
        else if (pull == 0)
            continue;

        if (fds[0].revents & POLLOUT)
        {
            if (fgets(buffer, sizeof(buffer), fp) != NULL)
            {
                if (sendto(clientFd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in6)) == -1)
                {
                    perror("sendto() failed\n");
                    close(clientFd);
                    fclose(fp);
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
    if (sendto(clientFd, buffer_exit, strlen(buffer_exit), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in6)) == -1)
    {
        perror("send() failed\n");
        close(clientFd);
        fclose(fp);
        exit(-1);
    }

    close(clientFd);
    fclose(fp);
}

/**
 * Connects to a server using IPv4 and Unix Domain Datagram protocol.
 * Reads data from a file named "data.txt" and sends it to the server.
 * @param serverIp The IP address of the server.
 * @param serverPort The port number of the server.
*/
void c_uds_dgram(char *serverIp, int serverPort)
{

    int clientFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (clientFd == -1)
    {
        perror("Could not create socket\n");
        exit(1);
    }

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    struct sockaddr_un serverAddress = {0};
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        perror("fopen() failed\n");
        exit(1);
    }

    struct timeval start;
    gettimeofday(&start, NULL);

    long buffer_time[MAX_EVENTS];
    buffer_time[0] = start.tv_sec;
    buffer_time[1] = start.tv_usec;

    if (sendto(clientFd, &buffer_time[0], sizeof(buffer_time[0]), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("sendto() failed\n");
        exit(1);
    }

    if (sendto(clientFd, &buffer_time[1], sizeof(buffer_time[1]), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("sendto() failed\n");
        exit(1);
    }

    char buffer[BUFFER_SIZE] = {0};
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sendto(clientFd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un)) == -1)
        {
            perror("sendto() failed\n");
            exit(1);
        }
    }

    fclose(fp);
    char buffer_exit[6] = "exit\n";
    if (sendto(clientFd, buffer_exit, strlen(buffer_exit), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un)) == -1)
    {
        perror("sendto() failed");
        exit(1);
    }

    close(clientFd);
}

/**
 * Connects to a server using Unix Domain Stream protocol.
 * Reads data from a file named "data.txt" and sends it to the server using a Unix Domain Stream socket.
*/
void c_uds_stream()
{
    int clientFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clientFd == -1)
    {
        perror("Socket failed\n");
        exit(1);
    }

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        perror("setsockopt() failed\n");
        exit(-1);
    }

    struct sockaddr_un serverAddress = {0};
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);
    int connectResult = connect(clientFd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr_un));
    if (connectResult == -1)
    {
        perror("connect() failed\n");
        close(clientFd);
        exit(-1);
    }

    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        perror("fopen() failed\n");
        close(clientFd);
        exit(1);
    }

    struct timeval start;
    gettimeofday(&start, NULL);

    long buffer_time[2] = {0};
    buffer_time[0] = start.tv_sec;
    buffer_time[1] = start.tv_usec;

    if (send(clientFd, &buffer_time[0], sizeof(buffer_time[0]), 0) == -1)
    {
        perror("send() failed\n");
        fclose(fp);
        close(clientFd);
        exit(-1);
    }

    if (send(clientFd, &buffer_time[1], sizeof(buffer_time[1]), 0) == -1)
    {
        perror("send() failed\n");
        fclose(fp);
        close(clientFd);
        exit(-1);
    }

    char buffer[BUFFER_SIZE] = {0};

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        ssize_t bytes_written = write(clientFd, buffer, strlen(buffer));
        if (bytes_written == -1)
        {
            perror("write() failed\n");
            fclose(fp);
            close(clientFd);
            exit(1);
        }
    }

    fclose(fp);
    close(clientFd);
}

/**
 * Sends data to a server using the Memory-Mapped File technique.
 * Reads data from a file named "data.txt" and sends it to the server.
 * @param serverPort The port number of the server.
*/
void c_mmap(int serverPort)
{

    struct stat filestat;
    int fd = open("data.txt", O_RDONLY);
    if (fd < 0)
    {
        perror("open() failed\n");
    }
    if (fstat(fd, &filestat) < 0)
    {
        perror("fstat() falied\n");
    }

    char *filedata = mmap(0, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (filedata == MAP_FAILED)
    {
        perror("mapp() failed");
    }

    int clientFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientFd == -1)
    {
        perror("socket");
    }

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        perror("setsockopt() failed");
        exit(-1);
    }

    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = serverPort;
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    struct timeval start;
    gettimeofday(&start, NULL);


    long buffer_time[2];
    buffer_time[0] = start.tv_sec;
    buffer_time[1] = start.tv_usec;

    
    if (sendto(clientFd, &buffer_time[0], sizeof(buffer_time[0]), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("sendto() failed\n");
        exit(1);
    }

    if (sendto(clientFd, &buffer_time[1], sizeof(buffer_time[1]), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("sendto() failed\n");
        exit(1);
    }

    size_t remaining = filestat.st_size;
    char *data_ptr = filedata;
    while (remaining > 0)
    {
        size_t send_size = remaining > 4096 ? 4096 : remaining;
        int n = sendto(clientFd, data_ptr, send_size, 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
        if (n < 0)
        {
            perror("sendto()  failed\n");
        }
        data_ptr += send_size;
        remaining -= send_size;
    }

    close(fd);
    close(clientFd);
}

/**
 * Sends data to a server using a named pipe (FIFO).
 * Reads data from a file named "data.txt" and sends it to the server.
*/
void c_pipe()
{
    
    FILE *file = fopen("data.txt", "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    int fd = open(FIFO_NAME, O_WRONLY);
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

    char buffer[BUFFER_SIZE] ={0};
    ssize_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        write(fd, buffer, bytes_read);
    }
    fclose(file);
    close(fd);
}

/**
 * Client B function that connects to a server and performs different operations based on the given parameters.
 * @param serverIp The IP address of the server.
 * @param serverPort The port number of the server.
 * @param type The type of connection protocol (e.g., "ipv4", "ipv6", "mmap", "pipe", "uds").
 * @param param The parameter associated with the connection protocol.
*/
void clientB(char *serverIp, int serverPort, char *type, char *param)
{
    // create file that 100mb size
    generate_data_to_file();

    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd == -1)
    {
        printf("Could not create socket : %d", errno);
        exit(-1);
    }

    int enableReuse = 1;
    int ret = setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }


    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = serverPort;

    int connectResult = connect(clientFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code : %d", errno);
        close(clientFd);
        exit(-1);
    }

    printf("message1: %s\n", type);
    printf("message2: %s\n", param);

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

    close(clientFd);

    sleep(1);

    if (!strcmp(type, "ipv4") && !strcmp(param, "tcp"))
        c_ipv4_tcp(serverIp, serverPort);

    else if (!strcmp(type, "ipv4") && !strcmp(param, "udp"))
    {
        c_ipv4_udp(serverIp, serverPort);
    }
    else if (!strcmp(type, "ipv6") && !strcmp(param, "tcp"))
        c_ipv6_tcp(serverIp, serverPort);

    else if (!strcmp(type, "ipv6") && !strcmp(param, "udp"))
        c_ipv6_udp(serverIp, serverPort);

    else if (!strcmp(type, "mmap") && !strcmp(param, "filename"))
        c_mmap(serverPort);

    else if (!strcmp(type, "pipe") && !strcmp(param, "filename"))
        c_pipe();

    else if (!strcmp(type, "uds") && !strcmp(param, "dgram"))
        c_uds_dgram(serverIp, serverPort);

    else if (!strcmp(type, "uds") && !strcmp(param, "stream"))
        c_uds_stream();
}
