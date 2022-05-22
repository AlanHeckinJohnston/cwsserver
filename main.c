#include <stdio.h>
#include <winsock2.h>
#include <stdint.h>

int main()
{
    WSADATA lpWSAData;

    WORD version = MAKEWORD(2,2);

    int startupResult = WSAStartup(version, &lpWSAData);

    if (startupResult != 0)
    {
        printf("Failed to initialize windows socket. Error code: %d", startupResult);
        return 1;
    }
    else
    {
        printf("Initialized WSA\n");
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (listenSocket == INVALID_SOCKET)
    {
        printf("Failed to create socket: %d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("Created socket\n");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8000);
    address.sin_addr.s_addr = inet_addr("0.0.0.0\0");
    int bound = bind(listenSocket, (struct sockaddr*) &address, sizeof(address));

    if (bound == SOCKET_ERROR)
    {
        printf("Sorry m8, failed to bind...\n");
        printf("%d", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("Bound socket\n");
    }

    printf("socket looks good to me kek\n");

    if (listen(listenSocket,SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Failed to listen: %s\n", WSAGetLastError());
        closesocket(listenSocket);
    }
    else{
        printf("Listening\n");
    }


    SOCKET clientSocket;

    clientSocket = accept(listenSocket, NULL, NULL);
    

    if (clientSocket == INVALID_SOCKET)
    {
        printf("Invalid socket: %d\n", WSAGetLastError());
    }

    printf("Socket connected.");
    
    int iResult;
    while (1)
    {
        char recvbuf[5000];
        iResult = recv(clientSocket, recvbuf, 5000, 0);

        if (iResult > 0)
        {
            printf("received %d bytes:\n", iResult);
            printf("%s\n", recvbuf);
        }
        else if (iResult == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            printf("UH OH UH OH\n");
            break;
        }
    }
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
