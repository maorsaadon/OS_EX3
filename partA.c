#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include "headers.h"

#define BUFFER_SIZE 1024

void serverA(int serverPort)
{
    // Create socket
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == -1)
    {
        printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress = {0}, clientAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = serverPort;

    // Bind socket to address and port
    int bindResult = bind(listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        printf("Bind failed with error code : %d", errno);
        close(listeningSocket);
        exit(-1);
    }
    else
        printf("executed Bind() successfully\n");

    // Listen for incoming connections
    int listenResult = listen(listeningSocket, 1);
    if (listenResult == -1)
    {
        printf("listen() failed with error code : %d", errno);
        close(listeningSocket);
        exit(-1);
    }
    else
        printf("Waiting for incoming TCP-connections...\n");

    socklen_t len_clientAddress = {0};
    len_clientAddress = sizeof(clientAddress);
    int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &len_clientAddress);
    if (clientSocket == -1)
    {
        printf("listen failed with error code : %d", errno);
        close(listeningSocket);
        exit(-1);
    }
    else
        printf("A new client connection accepted\n");

    close(listeningSocket);

    struct pollfd fds[2];
    fds[0].fd = clientSocket;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    // Send and receive messages
    char iSend[BUFFER_SIZE] = {0};
    while (1)
    {
        // Wait for events
        int pull = poll(fds, 2, -1);
        if (pull == -1)
        {
            printf("Pull failed with error code : %d", errno);
            exit(-1);
        }

        else
        {
            // Check if input from keyboard is available
            if (fds[1].revents & POLLIN)
            {
                // Read input from keyboard
                fgets(iSend, BUFFER_SIZE, stdin);
                iSend[strlen(iSend) - 1] = '\0';

                // Send message to server
                int bytesSent = send(clientSocket, iSend, strlen(iSend), 0);
                if (bytesSent == -1)
                    printf("send() failed with error code : %d\n", errno);
                else if (bytesSent == 0)
                    printf("peer has closed the TCP connection prior to send().\n");
            }

            // Check if server socket is readable
            if (fds[0].revents & POLLIN)
            {
                char iGet[BUFFER_SIZE] = {0};
                // Receive message from server
                int bytesReceived = recv(clientSocket, iGet, BUFFER_SIZE, 0);

                if (bytesReceived < 0)
                {
                    printf("client: something went wrong\n");
                    exit(-1);
                }
                else if (bytesReceived == 0)
                {
                    printf("Server disconnected.\n");
                    break;
                }
                printf("Client: %s\n", iGet);
            }
        }
    }

    // Close socket
    close(clientSocket);
}

void clientA(char *serverIp, int serverPort)
{
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket : %d", errno);
        exit(-1);
    }
    else
        printf("New socket opened\n");

    int enableReuse = 1;
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0)
    {
        printf("setsockopt() failed with error code : %d", errno);
        exit(-1);
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverIp);
    serverAddress.sin_port = htons(serverPort);

    // Connect to server
    int connectResult = connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code : %d", errno);
        close(sock);
        exit(-1);
    }
    else
        printf("connected to receiver\n");

    // Initialize pollfd array
    struct pollfd fds[2];
    fds[0].fd = sock;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    // Send and receive messages
    char iSend[BUFFER_SIZE] = {0};
    while (1)
    {
        // Wait for events
        int pull = poll(fds, 2, -1);
        if (pull == -1)
        {
            printf("Pull failed with error code : %d", errno);
            exit(-1);
        }

        else
        {
            // Check if input from keyboard is available
            if (fds[1].revents & POLLIN)
            {
                // Read input from keyboard
                fgets(iSend, BUFFER_SIZE, stdin);
                iSend[strlen(iSend) - 1] = '\0';

                // Send message to server
                int bytesSent = send(sock, iSend, strlen(iSend), 0);
                if (bytesSent == -1)
                    printf("send() failed with error code : %d\n", errno);
                else if (bytesSent == 0)
                    printf("peer has closed the TCP connection prior to send().\n");
            }

            // Check if server socket is readable
            if (fds[0].revents & POLLIN)
            {
                char iGet[BUFFER_SIZE] = {0};

                // Receive message from server
                int bytesReceived = recv(sock, iGet, BUFFER_SIZE, 0);

                if (bytesReceived < 0)
                {
                    printf("client: something went wrong\n");
                    exit(-1);
                }
                else if (bytesReceived == 0)
                {
                    printf("Server disconnected.\n");
                    break;
                }

                printf("Server: %s\n", iGet);
            }
        }
    }

    // Close socket
    close(sock);
}

