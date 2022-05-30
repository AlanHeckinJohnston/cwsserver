#ifndef SOCKET_H
#define SOCKET_H
#include <winsock2.h>

SOCKET createSocket(int* port, int* error_code, char*** error_string);

#endif