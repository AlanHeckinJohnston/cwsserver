
#include <stdio.h>
#include "client_management.h"
#include "websocket.h"
#include <time.h>


void printMessages(struct SocketInfo** sockets)
{
    for (int i = 0; i < 50; i++)
    {
        if (!(*sockets)[i].socket_populated || !(*sockets)[i].hasLastMessage)
        {
            continue;
        }
        printf("%s", (*sockets)[i].pendingMessage);
        printf("\r\n");
        (*sockets)[i].hasLastMessage = 0;
        free((*sockets)[i].pendingMessage);
    }
}

int completeHandshake(struct SocketInfo* socketInfo, int* errorCode)
{
    *errorCode = openConection(socketInfo);
    if (*errorCode == 0)
    {
        return 1;
    }
    return 0;
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
        (*sockets)[i].hasLastMessage = 0;
        (*sockets)[i].last_message_time = time(NULL);
        (*sockets)[i].handshake_completed = 0;
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
                (*sockets)[i].hasLastMessage = 1;
                (*sockets)[i].pendingMessage = malloc(sizeof(char)*receiveResult);
                (*sockets)[i].last_message_time = time(NULL);
                strcpy((*sockets)[i].pendingMessage, buffer);
                result++;
                readInfo -= 1;
            }
            else if (receiveResult <= 0)
            {
                (*sockets)[i].socket_populated = 0;
                if ((*sockets)[i].hasLastMessage)
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


int writeToClients(struct SocketInfo** sockets, fd_set* write, int* writeInfo)
{
    int result = 0;
    for (int i = 0; i < 50 && writeInfo > 0; i++)
    {
        if ((*sockets)[i].socket_populated && FD_ISSET((*sockets)[i].socket, writeInfo))
        {
            printf("writing to socket %d the following message: \n%s\n", i, (*sockets)[i].pendingMessage);

            int writeResult = send((*sockets)[i].socket, (*sockets)[i].pendingMessage, strlen((*sockets)[i].pendingMessage), MSG_OOB);
            printf("successfully wrote to %d", i);
            if (writeResult == SOCKET_ERROR)
            {
                // let's not write a message here
            }
            else
            {
                // if there isn't an error, we can clear the buffer though
                (*sockets)[i].pendingMessage = NULL;
            }
            writeInfo --;
        }
    }
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
            (*sockets)[i].hasLastMessage = 0;

            if ((*sockets)[i].hasLastMessage)
            {
                free((*sockets)[i].pendingMessage);
            }
            result[count] = i;
            count++;
        }
    }
}

void sendOnSocket(struct SocketInfo* socketInfo, char* msgBuffer)
{
    socketInfo->pendingMessage = msgBuffer;
}