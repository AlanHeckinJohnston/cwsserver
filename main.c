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

    u_long mode;
    
    if (listenSocket == INVALID_SOCKET)
    {
        printf("Failed to create socket: %d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("Created socket\n");
    }

    int success = ioctlsocket(listenSocket, FIONBIO, &mode); 

    if (success != 0)
    {
        printf("Could not switch socket mode. :(\n%d\n", WSAGetLastError());
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
    int socketPopulated = 0;

    fd_set read;
    fd_set write;

    while (1)
    {
        FD_ZERO(&read);
        FD_SET(listenSocket, &read);

        if (socketPopulated)
        {
            FD_SET(clientSocket, &read);
        }
        int readInfo = select(0, &read, NULL, NULL, NULL);
        
        if (readInfo == SOCKET_ERROR)
        {
            printf("error reading: %d\n", WSAGetLastError());
        }   
        else if (readInfo > 0)
        {
            if (FD_ISSET(listenSocket, &read))
            {
                clientSocket = accept(listenSocket, NULL, NULL);

                if (clientSocket == INVALID_SOCKET)
                {
                    printf("could not accept connection - %d", WSAGetLastError());
                }
                else
                {
                    printf("Accepted connection\n");
                    socketPopulated = 1;
                }
                readInfo -= 1;
            }
            
        
            if (socketPopulated && FD_ISSET(clientSocket, &read))
            {
                char buffer[100];
                int receiveResult = recv(clientSocket, buffer, 100, 0);
                if (receiveResult > 0)
                {
                    int len = receiveResult > 100 ? 100 : receiveResult;

                    buffer[len] = '\0';
                    printf("%s", buffer);
                }
                else
                {
                    socketPopulated = 0;
                    closesocket(clientSocket);
                    printf("\n");
                }
            }
        }
    }
    
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
