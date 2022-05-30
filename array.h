#ifndef ARRAY_H
#define ARRAY_H

// returns index of array string is located in, or -1.
int array_search_n(char** string, int length, char* search, int search_length);

// returns index of array string is located in, or -1.
int array_search(char** string, int length, char* search);
#endif