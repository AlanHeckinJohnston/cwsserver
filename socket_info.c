#include "socket_info.h"
#include <string.h>
#include <stdlib.h>

char* consume_message(struct SocketInfo* socket)
{
    char* result = malloc(strlen(socket->receivedMessage)*sizeof(char));
    strcpy(result, socket->receivedMessage);
    socket->receivedMessage = NULL;
    socket->hasReceivedMessage = 0;
    return result;
}