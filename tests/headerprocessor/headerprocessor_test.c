#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headerprocessor.h"
int main()
{
    FILE *file = fopen("tests/header_test.txt", "r");

    
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

    char** headers = malloc(sizeof(char*)*45);

    if (headers == NULL)
    {
        printf("Ran out of memory :(");
        return 1;
    }

    int headercount = getHeaders(s,headers);
    printf("%d headers loaded\n", headercount);
    for (int i = 0; i < headercount; i++)
    {
        printf("%d - %s\n",i, headers[i]);
    }


    char* host = getHeaderValue(headers, headercount, "Host");

    if (host != NULL)
    {
        printf("%s", host);
    }
}