#include "./string.h"

void strcat(char *destination, const char *source_first, const char *source_second)
{
    char *dest = destination;

    while (*source_first != '\0')
        *dest++ = *source_first++;

    while (*source_second != '\0')
        *dest++ = *source_second++;
    *dest = '\0';
}
int toupper(int ch)
{
    if (ch >= 'a' && ch <= 'z')
    {
        return ch - ('a' - 'A');
    }
    return ch;
}

char *strchr(const char *str, char c)
{
    
    const char *p = str;

    while (*p != '\0')
    {
        if (*p == c)
            return (char *)p;   
        p++;
    }

    return (char*)0x0;
}