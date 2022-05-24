
#include <stdio.h>
#include "client_management.h"



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
                strcpy((*sockets)[i].pendingMessage, buffer);
                result++;
                readInfo -= 1;
            }
            else if (receiveResult == 0)
            {
                (*sockets)[i].socket_populated = 0;
                free((*sockets)[i].pendingMessage);
                closesocket((*sockets)[i].socket);
                *connectedClients --;

                if (receiveResult == SOCKET_ERROR)
                {
                }
            }
        }
    }


    return result;
}
