#pragma once
#include "../x86/x86.h"

#include "../stdint/stdint.h"

#define DATA 0x1F0
#define FEATURES 0x1F1
#define SECTOR_COUNT 0x1F2
#define LBA_LOW 0x1F3
#define LBA_MID 0x1F4
#define LBA_HIGH 0x1F5
#define DRIVE_HEAD 0x1F6
#define STATUS_COMMAND 0x1F7

#define DISK_COMMAND_READ_SECTOR 0x20


extern void read_disk_asm(uint32_t LBA, uint8_t numberOfSectors, uint32_t bufferADR);

bool read(uint16_t *buffer_adr, uint64_t LBA, uint16_t sectorCount);