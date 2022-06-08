#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "socket.h"
#include "socket_info.h"

int confirmHandshakeAllowed(char** headers, int headerCount);
char* generateResponse(char** headers, int headerCount);
// returns 0 on success, or non zero denoting error code on error.
int openConection(struct SocketInfo* socket);
void b64_encode(char* string, char** out);
/**
 * Error codes -
 * 1 - invalid path or method - expect GET at /.
 * 2 - requested unavailable extension
 * 3 - no key provided 
 */
int confirmHandshakeAllowed(char** headers, int headerCount);
#endif