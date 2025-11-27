
#include "stdio/stdio.h"
#include "x86/x86.h"
#include "fat/fat.h"

void stage2_main()
{
    clear();
    printf("hello www\n");

    // ead_disk_asm(0, 1, (uint32_t)buffer);

    
    // disk_read((uint16_t*)0x11000, 0x204, 0x200);
    // disk_read((uint16_t*)0x12000, 0x204, 0x200);

    // printf("finished\n");
    // halt();
    // return;
    

    FAT fat;

    if (!fat_init(&fat))
    {
        printf("error inizilizing fat\n");
        halt();
        return;
    }

    printf("0x%x\n", fat.bootSector.reservedSectors * fat.bootSector.bytesPerSector);

    // printHexDump(&fat.bootSector, 10);

    // printf("0x%x\n", fat.bootSector.bytesPerSector);

    FAT_DirEntry entry;

    if (!fat_findFileInRoot(&fat, &entry, "kern", "o"))
    {
        printf("error searching for root entry in fat\n");
        halt();
        return;
    }


    printf("\nfinished reading\n");

    halt();
    while (1)
    {
    }
}
