#ifndef CLIENT_MANAGEMENT_H
#define CLIENT_MANAGEMENT_H
#include <winsock2.h>
#define SOCKET_TIMEOUT 10
#include "socket_info.h"


int readFromClients(struct SocketInfo** sockets, fd_set* read, int readInfo, int* connectedClients);
int acceptClients(SOCKET* listenSocketPtr, struct SocketInfo** sockets, int* connectedClients);
int writeToClients(struct SocketInfo** sockets, fd_set* write, int writeInfo);
void closeInactive(struct SocketInfo** sockets, int* result);
void printMessages(struct SocketInfo** sockets);

// msgBuffer MUST be NULL terminated
void sendOnSocket(struct SocketInfo* socketInfo, char* msgBuffer);

#endif