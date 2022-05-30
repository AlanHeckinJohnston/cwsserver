#ifndef HEADER_PROCESSOR_H
#define HEADER_PROCESSOR_H

char* getHeaderValue(char** headers, int headersLength, char* header);
int getHeaders(char* in, char** headers);


#endif