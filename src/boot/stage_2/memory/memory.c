#include "./memory.h"

bool memcmp(const void *lhs, const void *rhs, unsigned int count)
{
    if (count == 0)
        return true;

    for (int i = 0; i < count; i++)
    {
        if (*(uint8_t *)(lhs + i) != *(uint8_t *)(rhs + i))
            return false;
    }
    return true;
}

void *memcpy(void *destination, const void *source, uint64_t num)
{
    uint8_t* d= destination;
    uint8_t* s = source;

    for (int i = 0; i < num; i++)
        *d++ = *s++;

    return destination;
}