
#include "headers.h"

/**
 * Generates data and writes it to a file named "data.txt".
 * The generated data consists of random strings of length 1024 repeated multiple times.
 * Each string is written to the file using fprintf.
 */
FILE *generate_data_to_file()
{
    srand(time(NULL)); // Initialization, should only be called once.
    FILE *fp = fopen(FILE_NAME, "w+");
    if (fp == NULL)
    {
        perror("fopen() failed\n");
        exit(-1);
    }
    for (size_t i = 0; i < FILE_SIZE - 1; i++)
    {
        char randomChar = (rand() % 255) + 1;
        fprintf(fp, "%c", randomChar);
    }
    fprintf(fp, "%c", 0);
    fseek(fp, 0, SEEK_SET);

    return fp;
}

unsigned long hash(FILE *file)
{
    unsigned long sum = 0;
    int byte;

    while ((byte = getc(file)) != EOF)
    {
        sum += byte;
    }

    return sum;
}

/**
 * Connects to a server using IPv4 and TCP protocol.
 * Reads data from a file named "data.txt" and sends it to the server.
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
    int clientFd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
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
        close(clientFd);
        exit(-1);
    }

    struct sockaddr_in6 serverAddress = {0};
    serverAddress.sin6_family = AF_INET6;
    inet_pton(AF_INET6, (const char *)serverIp, &serverAddress.sin6_addr);
    serverAddress.sin6_port = htons(serverPort);

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

    // Calculate start time
    struct timeval start;
    gettimeofday(&start, NULL);
    // Create a buffer to hold the start time
    long start_time_buffer[2];
    start_time_buffer[0] = start.tv_sec;
    start_time_buffer[1] = start.tv_usec;

    // Send the start time to the server
    if (send(clientFd, &start_time_buffer[0], sizeof(start_time_buffer[0]), 0) == -1)
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
void c_mmap(char *param)
{

    int fd = open("data.txt", O_RDONLY);
    if (fd < 0)
    {
        perror("File open error");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0)
    {
        perror("File stat error");
        close(fd);
        exit(1);
    }

    size_t size = st.st_size;

    int shm_fd = shm_open(param, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0)
    {
        perror("Shared memory creation error");
        close(fd);
        exit(1);
    }

    ftruncate(shm_fd, size); // resize the shared mamory to the file size.

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("Memory mapping error");
        close(fd);
        shm_unlink(param);
        exit(1);
    }
    printf("size: %ld\n", size);
    printf("%ld\n", read(fd, addr, size));
    perror("read");

    close(fd);
}

/**
 * Sends data to a server using a named pipe (FIFO).
 * Reads data from a file named "data.txt" and sends it to the server.
 */
void c_pipe(char* param , FILE* fp)
{
    
    int fdFIFO;

    // Remove old FIFO
    unlink(param);

    // Create the FIFO
    if (mkfifo(param, 0666) == -1)
    {
        fclose(fp);
        exit(1);
    }

    // Open the FIFO for writing
    if ((fdFIFO = open(param, O_WRONLY)) == -1)
    {
        fclose(fp);
        unlink(param);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    // Read the file and write its contents to the FIFO
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)
    {
        write(fdFIFO, buffer, bytesRead);
    }

    // Close the file and the FIFO
    close(fdFIFO);
    fclose(fp);
    unlink(param);

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
    FILE* fp = generate_data_to_file();

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

    unsigned long checksum = hash(fp);
    if(send(clientFd, &checksum, sizeof(checksum), 0) == -1){

        printf("send() failed with error code : %d", errno);
        exit(1);
    }

    //need to be here(before socket close)
    if(!strcmp(type,"pipe")){
        param = FILE_NAME;
        if (send(clientFd, param, sizeof(param), 0) == -1)
        {
            printf("send() failed with error code : %d", errno);
            exit(-1);
        }

        c_pipe(param , fp);

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

    else if (!strcmp(type, "mmap"))
        c_mmap(param);

    else if (!strcmp(type, "uds") && !strcmp(param, "dgram"))
        c_uds_dgram(serverIp, serverPort);

    else if (!strcmp(type, "uds") && !strcmp(param, "stream"))
        c_uds_stream();
}
