#include "x86/x86.h"
#include "stdio/stdio.h"

void stage2_main()
{
    memset((void *)0xB8000, 0, 80 * 25);
    const char *x = "helloworld\0";


    

    //putc('x');

    uint32_t count = 0;
    while (1)
    {
        printf("count : 0x%x\n\0", count);
    }
}
