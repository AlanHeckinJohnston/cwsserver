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




    fd_set read;
    fd_set write;

    struct SocketInfo {
        SOCKET socket;
        char *lastMessage;
        char *pendingMessage;
        int socket_populated;
    };


    // listening socket will not be in this array
    struct SocketInfo sockets[50];

    for (int i = 0; i < 50; i++)
    {
        sockets[i].socket_populated = 0;
    }

    while (1)
    {
        FD_ZERO(&read);
        FD_SET(listenSocket, &read);
        for (int i = 0; i < 50; i++)
        {
            if (sockets[i].socket_populated)
            {
                FD_SET(sockets[i].socket, &read);
            }
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
                SOCKET clientSocket = accept(listenSocket, NULL, NULL);

                int i = 0;
                int found = 0;
                while (i < 50 && found == 0)
                {
                    if (sockets[i].socket_populated == 0)
                    {
                        found = 1;
                    }
                    else
                    {

                        i++;
                    }
                }

                if (found == 0)
                {
                    send(clientSocket, "Sorry - no room for new clients\0", 32, 0);
                    printf("Rejected connection due to no more new clients available");
                    closesocket(clientSocket);
                }

                if (clientSocket == INVALID_SOCKET)
                {
                    printf("could not accept connection - %d", WSAGetLastError());
                }
                else
                {
                    printf("Accepted connection and stored in %d\n", i);
                    sockets[i].socket = clientSocket;
                    sockets[i].socket_populated = 1;
                    readInfo -= 1;
                }
            }
            
            for (int i = 0; i < 50 && readInfo > 0; i++)
            {
                if (sockets[i].socket_populated && FD_ISSET(sockets[i].socket, &read))
                {
                    char buffer[100];
                    int receiveResult = recv(sockets[i].socket, buffer, 100, 0);
                    if (receiveResult > 0)
                    {
                        buffer[receiveResult] = '\0';
                        printf("%s", buffer);
                        readInfo -= 1;
                    }
                    else if (receiveResult == 0)
                    {
                        sockets[i].socket_populated = 0;
                        closesocket(sockets[i].socket);
                        printf("Socket closed\n");

                        if (receiveResult == SOCKET_ERROR)
                        {
                            printf("Socket read error: %d\n", WSAGetLastError());
                        }
                    }
                }
            }
        }
    }
    
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
