
struct SocketInfo {
        SOCKET socket;
        char *lastMessage;
        char *pendingMessage;
        int socket_populated;
};


SOCKET createSocket(int* port, int* error_code, char*** error_string);