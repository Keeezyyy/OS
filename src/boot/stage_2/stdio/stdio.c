#include "../stdio/stdio.h"

uint32_t pos = 0;
void putc(char c)
{
    if (c == '\n')
    {
        pos += 160 - (pos % 160);
        return;
    }

    char *charSlot = (char *)(VRAM + pos);

    *charSlot = c;
    *(charSlot + 1) = CHAR_COLOR_WHITE;

    pos += 2;
}

void clear()
{
    memset((void *)0xB8000, 0, 80 * 25);

    pos = 0;
}

void printHexDump(void *adr, uint16_t lines)
{

    uint8_t* byteAdr = adr;
    for (int i = 0; i < lines; i++)
    {
        printf("0x%x\0", (uint32_t)byteAdr);

        for (int k = 0; k < 8; k++)
        {
            printf(" ");
            putByte(*(byteAdr));
            printf(" ");
            //printf("  %b  ", *(uint8_t*)(adr));
            byteAdr++;
        }

        printf("\n\0");
        byteAdr++;
    }
}

void putByte(const unsigned char val)
{
    unsigned char high = (val >> 4) & 0x0F;
    unsigned char low = val & 0x0F;

    if (high < 0x0A)
        putc(high + '0');
    else
        putc(high - 0x0A + 'A');

    if (low < 0x0A)
        putc(low + '0');
    else
        putc(low - 0x0A + 'A');
}

void putS(const char *str)
{
    while (*str != '\0')
    {
        putc(*str++);
    }
}

void printf(const char *str, ...)
{
    va_list ap;
    va_start(ap, str);

    while (*str != '\0')
    {

        if (*str == '%')
        {
            str++;

            switch (*str)
            {
            case 'x':
            {

                uint32_t val = va_arg(ap, uint32_t);
                for (int i = 3; i >= 0; i--)
                {
                    putByte((val >> (i * 8)) & 0xFF);
                }
            }
            break;
            case 's':
            {
                char *s = va_arg(ap, char *);
                putS(s);
            }
            case 'c':
            {
                char c = va_arg(ap, char);
                putc(c);
            }
            case 'b':
            {
                uint8_t c = va_arg(ap, char);
                putByte(c);
            }
            break;

            default:
                break;
            }
        }
        else
        {
            putc(*str);
        }
        str++;
    }
}

void *memset(void *ptr, uint8_t value, uint32_t num)
{
    uint8_t *memptr = (uint8_t *)ptr;

    for (uint32_t i = 0; i != num; i++)
    {
        *memptr = value;
        memptr++;
    }

    return ptr;
};