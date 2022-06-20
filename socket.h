#ifndef SOCKET_H
#define SOCKET_H
#include <winsock2.h>
#include "socket_info.h"

SOCKET createSocket(int* port, int* error_code, char*** error_string);

#endif