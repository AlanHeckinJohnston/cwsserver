
#include <stdio.h>
#include "client_management.h"
#include "websocket.h"
#include <time.h>
#include <stdlib.h>

void prepareSets(struct SocketInfo** sockets, SOCKET* listenSocket, fd_set* read, fd_set* write)
{
    FD_ZERO(read);
    FD_ZERO(write);

    FD_SET(*listenSocket, read);
    for (int i = 0; i < 50; i++)
    {
        if ((*sockets)[i].socket_populated)
        {
            FD_SET((*sockets)[i].socket, read);

            if ((*sockets)[i].hasMessageToSend && (*sockets)[i].messageToSend != NULL)
            {
                FD_SET((*sockets)[i].socket, write);
            }
        }
    }
}

void printMessages(struct SocketInfo** sockets)
{
    for (int i = 0; i < 50; i++)
    {
        if (!(*sockets)[i].socket_populated || !(*sockets)[i].hasReceivedMessage)
        {
            continue;
        }
        printf("%s", (*sockets)[i].receivedMessage);
        FILE* f = fopen("test.txt", "w");
        if (f == NULL)
        {
            printf("\n\nUH OH\n\n");
        }
        int written = fwrite((*sockets)[i].receivedMessage, 1, strlen((*sockets)[i].receivedMessage), f);
        // printf("%d", written);
        fclose(f);
        (*sockets)[i].hasReceivedMessage = 0;
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
        (*sockets)[i].hasReceivedMessage = 0;
        (*sockets)[i].last_message_time = time(NULL);
        (*sockets)[i].handshake_completed = 0;
        (*sockets)[i].hasMessageToSend = 0;
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
            char buffer[10000];
            int receiveResult = recv((*sockets)[i].socket, buffer, 10000, 0);
            if (receiveResult > 0)
            {
                buffer[receiveResult] = '\0';
                (*sockets)[i].hasReceivedMessage = 1;
                (*sockets)[i].receivedMessage = malloc(sizeof(char)*receiveResult);
                (*sockets)[i].last_message_time = time(NULL);
                strcpy((*sockets)[i].receivedMessage, buffer);
                result++;
                readInfo -= 1;
            }
            else
            {
                (*sockets)[i].socket_populated = 0;
                if ((*sockets)[i].hasReceivedMessage)
                {
                    free((*sockets)[i].receivedMessage);
                }
                closesocket((*sockets)[i].socket);
                *connectedClients --;
            }
        }
    }


    return result;
}

void printMessagesSending(struct SocketInfo** sockets)
{
    for (int i = 0; i < 50; i++)
    {
        if ((*sockets)[i].socket_populated && (*sockets)[i].messageToSend != NULL && (*sockets)[i].hasMessageToSend)
        {
            printf("%s", (*sockets)[i].messageToSend);
        }
    }
}


int writeToClients(struct SocketInfo** sockets, fd_set* write, int* writeInfo, int* errorCode)
{
    int result = 0;
    for (int i = 0; i < 50 && writeInfo > 0; i++)
    {
        if ((*sockets)[i].socket_populated 
        && FD_ISSET((*sockets)[i].socket, write)
        && (*sockets)[i].hasMessageToSend)
        {
            int writeResult = send((*sockets)[i].socket, (*sockets)[i].messageToSend, strlen((*sockets)[i].messageToSend), MSG_OOB);
            if (writeResult == SOCKET_ERROR)
            {
                errorCode[i] = WSAGetLastError();
                (*sockets)[i].messageToSend = NULL;
                (*sockets)[i].hasMessageToSend = 0;
                // let's not write a message here
            }
            else
            {
                // if there isn't an error, we can clear the buffer though
                (*sockets)[i].messageToSend = NULL;
                (*sockets)[i].hasMessageToSend = 0;
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
            (*sockets)[i].hasReceivedMessage = 0;

            if ((*sockets)[i].hasReceivedMessage)
            {
                free((*sockets)[i].messageToSend);
            }
            result[count] = i;
            count++;
        }
    }
}

void queueMessageSend(struct SocketInfo* socketInfo, char* msgBuffer)
{
    if (socketInfo->hasMessageToSend)
    {
        strcat(socketInfo->messageToSend, msgBuffer);
        return;
    }

    socketInfo->hasMessageToSend = 1;
    socketInfo->messageToSend = msgBuffer;
}