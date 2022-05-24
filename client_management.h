#include <winsock2.h>

struct SocketInfo {
    SOCKET socket;
    char *lastMessage;
    char *pendingMessage;
    int socket_populated;
    int hasMessagePending;
};


int readFromClients(struct SocketInfo** sockets, fd_set* read, int readInfo, int* connectedClients);
int acceptClients(SOCKET* listenSocketPtr, struct SocketInfo** sockets, int* connectedClients);