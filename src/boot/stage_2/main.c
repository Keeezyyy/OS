
#include "stdio/stdio.h"
#include "x86/x86.h"
#include "fat/fat.h"

void stage2_main()
{
    clear();

    const char* name = "/boot/bin/kernel/core/kernel.o\0";
    //const char* name = "boot\0";
    //const char* name = "app.o\0";

    FAT_FILE f;

    if(!open((void*)0x10000, name, &f)){
        printf("something went wrong opening the file!\n");
    }

    printf("cluster start num : 0x%x\n", f.clusterStartNum);

    halt();
    while (1)
    {
    }
}
