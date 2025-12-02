#include "elf.h"

void loadProgramHeader(ELF_PROGRAM_HEADER *pHeader, void *file, uint32_t vaddr, uint32_t filesz, uint32_t offset)
{
    uint8_t *dest = (uint8_t *)vaddr;

    // printf("load program header : 0x%x\n", (uint32_t)dest);
    // printf("load program header : 0x%x\n", (uint32_t)vaddr);
    // printf("load program header : 0x%x\n", (uint32_t)filesz);
    // printf("load program header : 0x%x\n", (uint32_t)offset);
    // printf("load program header : 0x%x\n", (uint32_t)file);
    // printf("load program header : 0x%x\n", (uint32_t)((uint8_t *)file) + offset);

    printHexDump(((uint8_t *)file) + offset, 2);

    memcpy(dest, ((uint8_t *)file) + offset, filesz);

    printHexDump(dest, 2);
}

int execv(void *elfHeader)
{
    ELF_HEADER *elf = (ELF_HEADER *)elfHeader;

    /*
    uint32_t phOffset = *(uint32_t *)((uint8_t *)elf + 32);
    uint64_t kernelEntry = *(uint64_t *)((uint8_t *)elf + 0x18);
    uint64_t phNum = *(uint16_t *)((uint8_t *)elf + 0x38);
    */

    uint32_t phOffset = elf->e_phoff;
    uint64_t kernelEntry = elf->e_entry;
    uint64_t phNum = elf->e_phnum;



    for (int i = 0; i < phNum; i++)
    {
        ELF_PROGRAM_HEADER *programHeader = (ELF_PROGRAM_HEADER *)((uint8_t *)elf + phOffset + i * sizeof(ELF_PROGRAM_HEADER));
        loadProgramHeader(programHeader, elf, programHeader->p_vaddr, programHeader->p_filesz, programHeader->p_offset);
    }

    return kernelEntry;
}
