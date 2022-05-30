#include <stdio.h>
#include "headerprocessor.h"
#include "websocket.h"
#include <stdlib.h>
#include <string.h>
int main()
{
    FILE *file = fopen("header_test.txt", "r");

    
char* s = malloc(sizeof(char)*5000);
    s[0] = '\0';

    int end = 0;
    do
    {
        char line[500];
        fgets(line, 500, file);

        end = feof(file);

        if (!end)
            strcat(s,line);
    } while (!end);


    fclose(file);

    char** headers = malloc(sizeof(char*)*40);

    int headerCount = getHeaders(s,headers);
    char* buffer = malloc(sizeof(char)*10000);
    generateResponse(headers, headerCount, buffer);
    
}