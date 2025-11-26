
#include "stdio/stdio.h"
#include "x86/x86.h"
#include "disk/disk.h"

uint8_t x[10];

void stage2_main()
{
    clear();
    printf("hello www\n");
    
    read(x, 0, 1);
    
    printf("reading done\n");
    while (1)
    {

    }
}
