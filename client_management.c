
#include <stdio.h>
#include "client_management.h"
#include <time.h>


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

int acceptClients(SOCKET* listenSocketPtr, struct SocketInfo** sockets, int* connectedClients)
{
    SOCKET clientSocket = accept(*listenSocketPtr, NULL, NULL);

    int i = 0;
    int found = 0;
    while (i < 50 && found == 0)
    {
        if ((*sockets)[i].socket_populated == 0)
            found = 1;
        else 
            i++;
        
    }

    u_long mode = 1;
    if (found == 0)
    {
        send(clientSocket, "Sorry - no room for new clients\0", 32, 0);
        // printf("Rejected connection due to no more new clients available");
        closesocket(clientSocket);
    }
    else
    if (clientSocket == INVALID_SOCKET)
    {
        // printf("could not accept connection - %d", WSAGetLastError());
    }
    else if (ioctlsocket(clientSocket, FIONBIO, &mode) == SOCKET_ERROR)
    {
        // printf("could not switch socket blocking mode - %d", WSAGetLastError());
    }
    else
    {
        printf("Accepted connection and stored in %d\n", i);
        *connectedClients++;
        (*sockets)[i].socket = clientSocket;
        (*sockets)[i].socket_populated = 1;
        (*sockets)[i].hasMessagePending = 0;
        (*sockets)[i].last_message_time = time(NULL);
        return 1;
    }

    return 0;
}


int readFromClients(struct SocketInfo** sockets, fd_set* read, int readInfo, int* connectedClients)
{
    int result = 0;
    for (int i = 0; i < 50 && readInfo > 0; i++)
    {
        int socket_populated = (*sockets)[i].socket_populated;
        if ((*sockets)[i].socket_populated && FD_ISSET((*sockets)[i].socket, read))
        {
            char buffer[100];
            int receiveResult = recv((*sockets)[i].socket, buffer, 100, 0);
            if (receiveResult > 0)
            {
                buffer[receiveResult] = '\0';
                (*sockets)[i].hasMessagePending = 1;
                (*sockets)[i].pendingMessage = malloc(sizeof(char)*receiveResult);
                (*sockets)[i].last_message_time = time(NULL);
                strcpy((*sockets)[i].pendingMessage, buffer);
                result++;
                readInfo -= 1;
            }
            else if (receiveResult <= 0)
            {
                (*sockets)[i].socket_populated = 0;
                if ((*sockets)[i].hasMessagePending)
                {
                    free((*sockets)[i].pendingMessage);
                }
                closesocket((*sockets)[i].socket);
                *connectedClients --;
            }
        }
    }


    return result;
}


void closeInactive(struct SocketInfo** sockets, int* result)
{
    for (int i = 0; i < 10; i++)
    {
        result[i] = -1;
    }

    for (int i = 0, count = 0; i < 50 && count < 10; i++)
    {
        if ((*sockets)[i].socket_populated && time(NULL) - (*sockets)[i].last_message_time > SOCKET_TIMEOUT)
        {
            closesocket((*sockets)[i].socket);
            (*sockets)[i].socket_populated = 0;
            (*sockets)[i].hasMessagePending = 0;

            if ((*sockets)[i].hasMessagePending)
            {
                free((*sockets)[i].pendingMessage);
            }
            result[count] = i;
            count++;
        }
    }
}