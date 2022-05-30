#ifndef CLIENT_MANAGEMENT_H
#define CLIENT_MANAGEMENT_H
#include <winsock2.h>
#define SOCKET_TIMEOUT 10

struct SocketInfo {
    SOCKET socket;
    char *lastMessage;
    char *pendingMessage;
    int socket_populated;
    int hasMessagePending;
    time_t last_message_time;
};


int readFromClients(struct SocketInfo** sockets, fd_set* read, int readInfo, int* connectedClients);
int acceptClients(SOCKET* listenSocketPtr, struct SocketInfo** sockets, int* connectedClients);
void closeInactive(struct SocketInfo** sockets, int* result);
void printMessages(struct SocketInfo** sockets);

#endif