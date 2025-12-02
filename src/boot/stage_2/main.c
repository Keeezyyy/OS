
#include "stdio/stdio.h"
#include "x86/x86.h"
#include "fat/fat.h"
#include "elf/elf.h"

#define KERNEL_PATH "/kernel.o\0"

__attribute__((__cdecl))
int stage2_main()
{
    clear();

    const char *name = KERNEL_PATH;
    FAT_FILE f;
    FAT *fat;

    if (!open((void *)0x10000, name, &f, &fat))
    {
        printf("something went wrong opening the file!\n");
        halt();
    }

    void *fileBuffer = (void*)0x20000;

    if (!readFile(fileBuffer, &f, f.size, fat))
    {
        printf("something went wrong reading the file!\n");
        halt();
    }
    int entry = execv(fileBuffer);

    return entry;
}
