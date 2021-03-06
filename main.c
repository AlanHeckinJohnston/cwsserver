#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include "socket.h"
#include "client_management.h"

#define SERVER_TIMEOUT 4000


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
        sockets[i].hasMessageToSend = 0;
        sockets[i].hasReceivedMessage = 0;
        sockets[i].handshake_completed = 0;
    }

    int connectedClients = 0;

    while (connectedClients > 0 || time(NULL) - last_update < SERVER_TIMEOUT)
    {
        prepareSets(&sockets, &listenSocket, &read, &write);

        struct timeval t = {};
        int total = select(0, &read, &write, NULL, &t);
        
        if (total == SOCKET_ERROR)
        {
            printf("error selecting: %d\n", WSAGetLastError());
        }   
        else if (total > 0)
        {
            last_update = time(NULL);
            if (FD_ISSET(listenSocket, &read))
            {
                acceptClients(&listenSocket, &sockets, &connectedClients);
            }

            if (readFromClients(&sockets, &read, total, &connectedClients) > 0)
            {
                printMessages(&sockets);

                // check for handhsakes 
                for (int i = 0; i < 50; i++)
                {
                    if (sockets[i].socket_populated == 1 && sockets[i].handshake_completed == 0)
                    {
                        // if handshake has not been done, this is probably it.

                        int errorCode;
                        if (!completeHandshake(&(sockets[i]), &errorCode))
                        {
                            printf("could not complete handshake - error %d", errorCode);
                        }
                        sockets[i].handshake_completed = 1;
                        printf("handshake queued\n");
                    }
                }
            }
            
            if (write.fd_count > 0)
            {
                if (total > 0)
                {
                    last_update = time(NULL);
                    printMessagesSending(&sockets);
                    int error[50];
                    writeToClients(&sockets, &write, &total, (int*) &error);

                    
                }
            }
        }
    }
    
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
