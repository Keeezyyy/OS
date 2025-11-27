#include "./string.h"

void strcat(char *destination, const char *source_first, const char *source_second)
{
    char* dest = destination;

    while (*source_first != '\0')
        *dest++ = *source_first++;

    while (*source_second != '\0')
        *dest++ = *source_second++;
    *dest = '\0';

}