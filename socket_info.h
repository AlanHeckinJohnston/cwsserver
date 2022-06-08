#ifndef SOCKET_INFO_H
#define SOCKET_INFO_H

struct SocketInfo {
    SOCKET socket;
    char *lastMessage;
    char *pendingMessage;
    int socket_populated;
    int hasLastMessage;
    time_t last_message_time;
    int handshake_completed;
};

#endif