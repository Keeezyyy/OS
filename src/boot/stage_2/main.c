
#include "stdio/stdio.h"
#include "x86/x86.h"
#include "disk/disk.h"

uint8_t x[512];

void stage2_main()
{
    clear();
    printf("hello www\n");

    uint16_t *buffer = (void*)0x10000;
    
    //ead_disk_asm(0, 1, (uint32_t)buffer);
    
    read(buffer, 0, 1);

    putByte(*buffer);
    printf("\n");
    
    halt();
    while (1)
    {

    }
}
