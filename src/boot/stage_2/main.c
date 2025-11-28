
#include "stdio/stdio.h"
#include "x86/x86.h"
#include "fat/fat.h"

void stage2_main()
{
    clear();

    const char *name = "app.o\0";
    FAT_FILE f;

    FAT *fat;

    if (!open((void *)0x20000, name, &f, &fat))
    {
        printf("something went wrong opening the file!\n");
        halt();
    }

    printf("cluster start num : 0x%x\n", f.clusterStartNum);
    printf("Size: 0x%x\n", f.size);
    printf("BPS: 0x%x\n", fat->bootSector.bytesPerSector);

    if (!readFile((void *)0x20000, &f, f.size, fat))
    {
        printf("something went wrong reading the file!\n");
        halt();
    }

    halt();
    while (1)
    {
    }
}
