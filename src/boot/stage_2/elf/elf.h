#pragma once
#include "../stdint/stdint.h"
#include "../memory/memory.h"
#include "../stdio/stdio.h"


#pragma pack(push, 1)
typedef struct
{
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} ELF_HEADER;
#pragma pack(pop)

typedef struct __attribute__((packed))
{
    uint32_t p_type;    
    uint32_t p_flags;   
    uint64_t p_offset;  
    uint64_t p_vaddr;   
    uint64_t p_paddr;   
    uint64_t p_filesz;  
    uint64_t p_memsz;   
    uint64_t p_align;   
} ELF_PROGRAM_HEADER;


void loadProgramHeader(ELF_PROGRAM_HEADER *pHeader, void *file, uint32_t vaddr, uint32_t filesz, uint32_t offset);
