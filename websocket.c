#include "websocket.h"
#include "headerprocessor.h"
#include "client_management.h"
#include "lib/base64/cencode.h"
#include "lib/sha1/sha1.h"
#include <stdio.h>
#include <stdlib.h>
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

void b64_encode(char* string, char** out)
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

void generateWebsocketAccept(char* websocketKey, char** buffer)
{
    char* fullstring = malloc(sizeof(char)*(strlen(websocketKey) + strlen(GUID)+1));

    strcpy(fullstring, websocketKey);
    strcat(fullstring, GUID);

    char sha1[21];

    SHA1(sha1, fullstring, strlen(fullstring));

    b64_encode(sha1, buffer);
}

static char* clampResponseHeader(char* responseHeader)
{
    char lastChar, curChar;
    int i = 0;
    while (!(lastChar == ' ' && curChar == ' '))
    {
        lastChar = curChar;
        curChar = responseHeader[i];
        i++;
    }

    // responseHeader += i;
    // *responseHeader = 0;
    return responseHeader; 
}

static char* getResponseHeader()
{
    FILE* f = fopen("resources/response_handshake_header.txt", "r");

    // something something file not there? bad install. skipping

    char* s = malloc(sizeof(char)*5000);
    s[0] = 0;
    int end = 0;
    do
    {
        char line[500];
        fgets(line, 500, f);

        end = feof(f);

        if (strlen(line) > 0)
            strcat(s,line);

    } while (!end);

    fclose(f);
    return clampResponseHeader(s);
}

static char* popResponseHeader(char** header, char* key)
{
    char* location = strstr(*header, "${key}");

    strcpy(location, key);
    

    char* end = location + strlen(key);
    *end = 0;

}

char* generateResponse(char** headers, int headerCount)
{
    char* key = getHeaderValue(headers, headerCount, "Sec-WebSocket-Key");
    if (key == NULL)
    {
        return NULL;
    }
    
    char* acceptBuffer = malloc(sizeof(char)*1000);
    generateWebsocketAccept(key, &acceptBuffer);

    char* messageBuffer = getResponseHeader();
    popResponseHeader(&messageBuffer, acceptBuffer);
    return messageBuffer;
}

int openConection(struct SocketInfo* socket, char* message)
{
    char** headers = malloc(sizeof(char*)*40);
    int headerCount = getHeaders(message, headers);

    int requestErrorCcode = confirmHandshakeAllowed(headers, headerCount);

    if (requestErrorCcode == 0)
    {
        // everything is good!
        char* message = generateResponse(headers, headerCount);
        // send(socket->socket, responseMessage, msgLength);
    }

    return 1;
}