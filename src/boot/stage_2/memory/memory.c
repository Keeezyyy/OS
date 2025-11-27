#include "./memory.h"

bool memcmp(const void *lhs, const void *rhs, unsigned int count)
{
    if (count == 0)
        return true;

    for (int i = 0; i < count; i++)
    {
        if (*(uint8_t*)(lhs + i) != *(uint8_t*)(rhs + i))
            return false;
    }
    return true;
}