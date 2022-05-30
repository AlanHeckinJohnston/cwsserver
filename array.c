#include "array.h"
#include <string.h>
#include <stdio.h>

int array_search_n(char** string, int length, char* search, int search_length)
{
    for (int i = 0 ; i < length; i++)
    {
        if (strncmp(string[i], search, search_length))
        {
            return i;
        }
    }

    return -1;
}

int array_search(char** string, int length, char* search)
{
    for (int i = 0 ; i < length; i++)
    {
        if (strcmp(string[i], search) == 0)
        {
            return i;
        }
    }

    return -1;
}