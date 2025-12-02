
#include "stdio/stdio.h"
#include "x86/x86.h"
#include "fat/fat.h"
#include "elf/elf.h"
#include "memory/memory.h"

#define KERNEL_PATH "/kernel.o\0"
#define RAM_MAP 0x50000
#define LOAD_RETRIES 10

__attribute__((__cdecl)) int stage2_main()
{
    clear();

    const char *name = KERNEL_PATH;
    FAT_FILE f;
    FAT *fat;

    void *fileBuffer = (void *)0x20000;

    bool worked = false;

    for (int i = 0; i < LOAD_RETRIES; i++)
    {
        if (!open((void *)0x10000, name, &f, &fat))
        {
            printf("e1\n");
        }

        if (readFile(fileBuffer, &f, f.size, fat))
        {
            worked = true;
            break;
        }
        printf("e2!\n");
    }

    if (!worked)
        halt();

    int entryAddress = execv(fileBuffer);

    printf("entry: 0x%x\n", entryAddress);

    return entryAddress;
}
