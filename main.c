#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "socket.h"

int main()
{
    time_t last_update = time(NULL);

    int port = 8000;
    int error_code;
    char** error_string = (char**) malloc(sizeof(char**)*1024);

    SOCKET listenSocket = createSocket(&port, &error_code, &error_string);

    if (listenSocket == INVALID_SOCKET)
    {
        printf("Socket failed: %s", *error_string);
        return 1;
    }
    else
    {
        printf("Successfully bound socket\n");
        free(error_string);
    }

    fd_set read;
    fd_set write;

    // server socket will not be in this array
    struct SocketInfo sockets[50];

    for (int i = 0; i < 50; i++)
    {
        sockets[i].socket_populated = 0;
    }

    int connectedClients = 0;

    while (connectedClients > 0 || time(NULL) - last_update < 120)
    {
        FD_ZERO(&read);
        FD_SET(listenSocket, &read);
        for (int i = 0; i < 50; i++)
        {
            if (sockets[i].socket_populated)
            {
                FD_SET(sockets[i].socket, &read);
            }
        }
        struct timeval t = {};
        int readInfo = select(0, &read, NULL, NULL, &t);
        
        if (readInfo == SOCKET_ERROR)
        {
            printf("error reading: %d\n", WSAGetLastError());
        }   
        else if (readInfo > 0)
        {
            last_update = time(NULL);
            if (FD_ISSET(listenSocket, &read))
            {
                SOCKET clientSocket = accept(listenSocket, NULL, NULL);

                int i = 0;
                int found = 0;
                while (i < 50 && found == 0)
                {
                    if (sockets[i].socket_populated == 0)
                    {
                        found = 1;
                    }
                    else
                    {

                        i++;
                    }
                }

                if (found == 0)
                {
                    send(clientSocket, "Sorry - no room for new clients\0", 32, 0);
                    printf("Rejected connection due to no more new clients available");
                    closesocket(clientSocket);
                }

                u_long mode = 1;
                if (clientSocket == INVALID_SOCKET)
                {
                    printf("could not accept connection - %d", WSAGetLastError());
                }
                else if (ioctlsocket(clientSocket, FIONBIO, &mode) == SOCKET_ERROR)
                {
                    printf("could not switch socket blocking mode - %d", WSAGetLastError());
                }
                else
                {
                    printf("Accepted connection and stored in %d\n", i);
                    connectedClients++;
                    sockets[i].socket = clientSocket;
                    sockets[i].socket_populated = 1;
                    readInfo -= 1;
                }
            }
            
            for (int i = 0; i < 50 && readInfo > 0; i++)
            {
                if (sockets[i].socket_populated && FD_ISSET(sockets[i].socket, &read))
                {
                    char buffer[100];
                    int receiveResult = recv(sockets[i].socket, buffer, 100, 0);
                    if (receiveResult > 0)
                    {
                        buffer[receiveResult] = '\0';
                        printf("%s", buffer);
                        readInfo -= 1;
                    }
                    else if (receiveResult == 0)
                    {
                        sockets[i].socket_populated = 0;
                        closesocket(sockets[i].socket);
                        connectedClients --;
                        printf("Socket closed\n");

                        if (receiveResult == SOCKET_ERROR)
                        {
                            printf("Socket read error: %d\n", WSAGetLastError());
                        }
                    }
                }
            }
        }
    }
    
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
