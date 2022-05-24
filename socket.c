#include <winsock2.h>
#include <stdio.h>
#include "socket.h"

static int initializeWindowsSocket(int* error_code, char*** error_string)
{
    WSADATA lpWSAData;

    WORD version = MAKEWORD(2,2);
    int startupResult = WSAStartup(version, &lpWSAData);

    if (startupResult != 0)
    {
        sprintf(**error_string, "Failed to initialize windows socket. Error code: %d", startupResult);
        return 0;
    }
    else
    {
        return 1;
    }
}

static SOCKET setNonBlocking(SOCKET* socket, int* error_code, char*** error_string)
{
    u_long mode = 1;

    int success = ioctlsocket(*socket, FIONBIO, &mode); 

    if (success == SOCKET_ERROR)
    {
        sprintf(**error_string, "Could not switch socket mode. :( - %d\n\0", WSAGetLastError());
        return INVALID_SOCKET;
    }

    return *socket;
}

static SOCKET init(int* error_code, char*** error_string)
{
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        *error_code = WSAGetLastError();
        printf("%d", *error_code);
        int printed = sprintf(**error_string, "Failed to create socket: %d\n\0", *error_code);
        return INVALID_SOCKET;
    }
    return listenSocket;
}

static int bindSocket(SOCKET* result, int* port, int* error_code, char*** error_string)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8000);
    address.sin_addr.s_addr = inet_addr("0.0.0.0\0");
    int bound = bind(*result, (struct sockaddr*) &address, sizeof(address));

    if (bound == SOCKET_ERROR)
    {
        sprintf(**error_string, "Sorry m8, failed to bind... %d\n", WSAGetLastError());
        closesocket(*result);
        return 0;
    }
    else
    {
        return 1;
    }
}

static int listenSocket(SOCKET* result, int* error_code, char*** error_string)
{
    if (listen(*result,SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Allocated memory");
        sprintf(**error_string, "Failed to listen: %d\n\0", WSAGetLastError());
        closesocket(*result);
    }
    
    return 1;
}

SOCKET createSocket(int* port, int* error_code, char*** error_string)
{
    if (initializeWindowsSocket(error_code, error_string) == 0) return INVALID_SOCKET;
    SOCKET result = init(error_code, error_string);
    if (result == INVALID_SOCKET) return INVALID_SOCKET;
    if (setNonBlocking(&result, error_code, error_string) == 0) return INVALID_SOCKET;
    if (bindSocket(&result, port, error_code, error_string) == 0) return INVALID_SOCKET;
    if (listenSocket(&result, error_code, error_string) == 0) return INVALID_SOCKET;
    return result;
}