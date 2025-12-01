
#include "stdio/stdio.h"
#include "x86/x86.h"
#include "fat/fat.h"
#include "elf/elf.h"

/*
  printf("offset 0x%x\n", *(uint32_t *)((uint8_t *)elf + 0x20));
  printf("offset 0x%x\n", *(uint16_t *)((uint8_t *)elf + 0x36));
  printf("offset 0x%x\n", *(uint16_t *)((uint8_t *)elf + 0x38));
*/
__attribute__((__cdecl))
int stage2_main()
{
    clear();

    // const char *name = "/boot/bin/kernel/core/kernel.o\0";
    const char *name = "/kernel.o\0";
    FAT_FILE f;
    FAT *fat;

    if (!open((void *)0x10000, name, &f, &fat))
    {
        printf("something went wrong opening the file!\n");
        halt();
    }

    if (!readFile((void *)0x20000, &f, f.size, fat))
    {
        printf("something went wrong reading the file!\n");
        halt();
    }



    //TODO make the struct parsing working
    ELF_HEADER *elf = (ELF_HEADER *)0x20000;

    uint32_t phOffset = *(uint32_t *)((uint8_t *)elf + 32);
    uint64_t kernelEntry = *(uint64_t *)((uint8_t *)elf + 0x18);
    
    ELF_PROGRAM_HEADER *programHeader = (ELF_PROGRAM_HEADER *)((uint8_t *)elf + phOffset);
    
    uint32_t filesz = *(uint32_t *)((uint8_t *)programHeader + 0x20);


    printf("program header num 0x%x\n", *(uint16_t *)((uint8_t *)elf + 0x38));
    printf("program entry 0x%x\n", *(uint64_t *)((uint8_t *)elf + 0x18));

    printf("filesz 0x%x\n", filesz);
    printf("offset in file:  0x%x - 0x%x\n", programHeader->p_offset, programHeader->p_offset + filesz);

    loadProgramHeader(programHeader, elf, *(uint32_t *)((uint8_t *)programHeader + 0x10), filesz, programHeader->p_offset);
    

    return kernelEntry;
}
