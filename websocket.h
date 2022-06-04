#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "socket.h"
#include "client_management.h"

int confirmHandshakeAllowed(char** headers, int headerCount);
void generateWebsocketAccept(char* websocketKey, char** buffer);
char* generateResponse(char** headers, int headerCount);
int openConection(struct SocketInfo* socket, char* message);
void b64_encode(char* string, char** out);
/**
 * Error codes -
 * 1 - invalid path or method - expect GET at /.
 * 2 - requested unavailable extension
 * 3 - no key provided 
 */
int confirmHandshakeAllowed(char** headers, int headerCount);
#endif