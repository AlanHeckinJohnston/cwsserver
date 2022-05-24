#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "socket.h"
#include "client_management.h"


void printMessages(struct SocketInfo** sockets)
{
    for (int i = 0; i < 50; i++)
    {
        if (!(*sockets)[i].socket_populated || !(*sockets)[i].hasMessagePending)
        {
            continue;
        }
        printf("%s", (*sockets)[i].pendingMessage);
        (*sockets)[i].hasMessagePending = 0;
        free((*sockets)[i].pendingMessage);
    }
}

int main()
{
    time_t last_update = time(NULL);

    int port = 8000;
    int error_code;
    char** error_string = malloc(sizeof(char**)*1024);

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
    struct SocketInfo* sockets = malloc(sizeof(struct SocketInfo) * 50);

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

                u_long mode = 1;
                if (found == 0)
                {
                    send(clientSocket, "Sorry - no room for new clients\0", 32, 0);
                    printf("Rejected connection due to no more new clients available");
                    closesocket(clientSocket);
                }
                else
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
                    sockets[i].hasMessagePending = 0;
                    readInfo -= 1;
                    printf("Allocated\n");
                }
            }

            if (readFromClients(&sockets, &read, readInfo, &connectedClients) > 0)
            {
                printMessages(&sockets);
            } 
        }
    }
    
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
