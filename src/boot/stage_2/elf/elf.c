#include "elf.h"

void loadProgramHeader(ELF_PROGRAM_HEADER *pHeader, void *file, uint32_t vaddr, uint32_t filesz, uint32_t offset)
{
    uint8_t *dest = (uint8_t *)vaddr;

    //printf("load program header : 0x%x\n", (uint32_t)dest);
    //printf("load program header : 0x%x\n", (uint32_t)vaddr);
    //printf("load program header : 0x%x\n", (uint32_t)filesz);
    //printf("load program header : 0x%x\n", (uint32_t)offset);
    //printf("load program header : 0x%x\n", (uint32_t)file);
    //printf("load program header : 0x%x\n", (uint32_t)((uint8_t *)file) + offset);

    
    printHexDump(((uint8_t *)file) + offset, 2);

    memcpy(dest, ((uint8_t *)file) + offset, filesz);

    printHexDump(dest, 2);
}