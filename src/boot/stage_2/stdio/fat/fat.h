#pragma once
#include "../../stdint/stdint.h"

#define DataRegister 0x1F0
#define SectorCount 0x1F2
#define LBAlow 0x1F3
#define LBAmid 0x1F4
#define LBAhigh 0x1F5
#define Drive 0x1F6
#define Command 0x1F7

struct
{

} FILE;

typedef enum
{
    READ_BINARY,
    READ,
    WRITE
} file_access;

struct FILE fopen(const char *name, file_access access);

void readSector(void *ptr, uint16_t LBA);
