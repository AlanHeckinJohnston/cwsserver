#ifndef SOCKET_INFO_H
#define SOCKET_INFO_H
#include <winsock2.h>

struct SocketInfo {
    SOCKET socket;
    char *receivedMessage;
    char *messageToSend;
    int socket_populated;
    int hasReceivedMessage;
    int hasMessageToSend;
    time_t last_message_time;
    int handshake_completed;
};

char* consume_message(struct SocketInfo* socket);

#endif