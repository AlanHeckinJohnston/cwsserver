#include "websocket.h"
#include "headerprocessor.h"
#include "client_management.h"
#include "lib/base64/cencode.h"
#include "lib/sha1/sha1.h"
#include <stdio.h>
#define PATH "GET / "

#define GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"


int correctPath(char* header)
{
    return (strstr(header, PATH) != NULL);
}

int confirmHandshakeAllowed(char** headers, int headerCount)
{
    if (!correctPath(headers[0]))
    {

        return 1;
    }

    if (getHeaderValue(headers, headerCount, "Sec-WebSocket-Extensions") != NULL)
    {
        return 2;
    }

    if (getHeaderValue(headers, headerCount, "Sec-Websocket-Key") == NULL)
    {
        return 3;
    }

    return 0;
}

int b64_encode(char* string, char** out)
{
    base64_encodestate st;
    *out = malloc(sizeof(char)*10000);
    char* c = *out;
    
    int cnt = 0;
    base64_init_encodestate(&st);
    cnt = base64_encode_block(string, strlen(string), *out, &st);
    c += cnt;
    cnt = base64_encode_blockend(c, &st);
    c += cnt;
    *c = '\0';
}

int generateWebsocketAccept(char* websocketKey, char* buffer)
{
    char* fullstring = malloc(sizeof(char)*(strlen(websocketKey) + strlen(GUID)));

    strcpy(fullstring, websocketKey);
    strcat(fullstring, GUID);

    char* b64 = malloc(sizeof(char) * 256);
    b64_encode(fullstring, &b64);
    SHA1(buffer, b64, strlen(b64));



    return 0;
}

int generateResponse(char** headers, int headerCount, char* messageBuffer)
{
    char* key = getHeaderValue(headers, headerCount, "Sec-WebSocket-Key");
    if (key == NULL)
    {
        return 0;
    }

    char* buffer = malloc(sizeof(char*)*512);
    generateWebsocketAccept(key, buffer);

    printf("%s", buffer);
}

int openConection(struct SocketInfo* socket, char* message)
{
    char** headers = malloc(sizeof(char*)*40);
    int headerCount = getHeaders(message, headers);

    int requestErrorCcode = confirmHandshakeAllowed(headers, headerCount);

    if (requestErrorCcode == 0)
    {
        // everything is good!
        char* responseMessage = malloc(sizeof(char)*5000);
        int msgLength = generateResponse(headers, headerCount, responseMessage);
        // send(socket->socket, responseMessage, msgLength);
        free(responseMessage);
    }

    return 1;
}