#include "headerprocessor.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "array.h"



char* getHeaderValue(char** headers, int headersLength, char* header)
{
    
    int location = array_search(headers, headersLength, header);

    if (location == -1)
    {
        return NULL;
    }
    
    return (char*) (headers[location + 1]);
}

//debugging function
static void formatChar(char c, char* out)
{
    if (c == '\n')
    {
        out[0] = '\\';
        out[1] = 'n';
        out[2] = '\0';
    }
    else if (c == '\r')
    {
        out[0] = '\\';
        out[1] = 'r';
        out[2] = '\0';
    }
    else
    {
        out[0] = c;
        out[1] = '\0';
    }
}

int getHeaders(char* in, char** headers)
{
    int headerIndex = 0;

    char curchar = in[0];
    char lastchar = '\0';

    char current[800] = {'\0'};

    int i = 0;
    
    int looping = 1;
    int colonConsumed = 0;
    while (looping)
    {
        curchar = in[i];
        if (
            
                   ((curchar == '\n') 
                || (curchar == ':' && !colonConsumed) 
                || curchar == '\0')
            
            && strlen(current) > 0
           )
        {
            // end current string - allocate space and save it to out array


            int length = strlen(current)+1;
            int size = sizeof(char)*(length);
            char* address = malloc(size);
            
            headers[headerIndex] = address;
            strcpy(headers[headerIndex], current);

            if (curchar == '\0')
                looping = 0;

            // restart new string
            *current = '\0';

            if (curchar != '\0'){
                headerIndex++;
            }

            if (curchar == ':')
                colonConsumed = 1;
            else if (curchar == '\n')
                colonConsumed = 0;
            else if (curchar == '\0')
                looping = 0;
        }
        else
        {
            if (curchar != '\r' && curchar != '\n' && !(curchar == ' ' && (lastchar == '\0' || lastchar == ':')))
                strncat(current, &curchar, 1);
        }

        lastchar = curchar;
        i++;
    }

    return headerIndex;
}


