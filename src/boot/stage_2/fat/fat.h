#pragma once
#include "../stdint/stdint.h"
#include "../disk/disk.h"
#include "../stdio/stdio.h"
#include "../memory/memory.h"
#include "../string/string.h"

#define BOOTSECTOR_LBA 0x0

typedef struct __attribute__((packed))
{
    uint8_t jumpCode[3]; // 00h Jump instruction (3 bytes)
    char OEMName[8];     // 03h OEM Name (8 bytes)

    uint16_t bytesPerSector;    // 0Bh Bytes per sector
    uint8_t sectorsPerCluster;  // 0Dh Sectors per cluster
    uint16_t reservedSectors;   // 0Eh Reserved sectors
    uint8_t numberOfFATs;       // 10h Number of FAT copies
    uint16_t maxRootDirEntries; // 11h Max number of root dir entries
    uint16_t smallSectorCount;  // 13h Total sectors if < 32MB
    uint8_t mediaDescriptor;    // 15h Media descriptor
    uint16_t sectorsPerFAT;     // 16h Sectors per FAT
    uint16_t sectorsPerTrack;   // 18h Geometry: sectors/track
    uint16_t numberOfHeads;     // 1Ah Geometry: number of heads
    uint32_t hiddenSectors;     // 1Ch Hidden sectors
    uint32_t totalSectors;      // 20h Total sectors if > 32MB

    // --- FAT16 Extended BPB ---
    uint8_t driveNumber;       // 24h Drive number
    uint8_t reserved1;         // 25h Reserved (always 0)
    uint8_t extendedSignature; // 26h Extended boot signature (0x29)
    uint32_t serialNumber;     // 27h Volume serial number
    char volumeLabel[11];      // 2Bh Volume label
    char fatType[8];           // 36h FAT type string (e.g. "FAT16   ")
} FAT16_BootSector;

typedef struct __attribute__((packed))
{
    uint8_t DIR_Name[11];     // 0x00  Short name (8.3 format)
    uint8_t DIR_Attr;         // 0x0B  File attributes
    uint8_t DIR_NTRes;        // 0x0C  Reserved, must be 0
    uint8_t DIR_CrtTimeTenth; // 0x0D  Creation time (tenths of a second)
    uint16_t DIR_CrtTime;     // 0x0E  Creation time
    uint16_t DIR_CrtDate;     // 0x10  Creation date
    uint16_t DIR_LstAccDate;  // 0x12  Last access date
    uint16_t DIR_FstClusHI;   // 0x14  High word of first cluster (0 for FAT16)
    uint16_t DIR_WrtTime;     // 0x16  Last write time
    uint16_t DIR_WrtDate;     // 0x18  Last write date
    uint16_t DIR_FstClusLO;   // 0x1A  Low word of first cluster
    uint32_t DIR_FileSize;    // 0x1C  File size in bytes
} FAT_DirEntry;

typedef struct
{
    uint16_t fatEntry[256];
} FAT_BLOCK;

typedef struct
{
    char name[8];
    char ext[3];
} FAT_FILENAME;

typedef union
{
    FAT_DirEntry dirEntries[16];
    FAT_BLOCK fatBlock;
} BUFFER_BLOCK;

typedef struct
{
    FAT16_BootSector bootSector;
    uint64_t fatStartInBytes;
    uint64_t rootStartInBytes;
    uint64_t dataStartInBytes;

    uint8_t numOfFats;

    BUFFER_BLOCK readBufferBlock;
} FAT;

typedef struct
{
    uint64_t size;
    uint32_t clusterStartNum;
} FAT_FILE;

bool fat_init(FAT *f);

bool fat_findFileInDir(FAT *f, uint8_t *bBlock, FAT_DirEntry *out, const char *const name);


//TODO remove the alloc solution :)
bool open(void *buffer, const char *path, FAT_FILE *out, FAT **out_FAT);

bool readFile(void *buffer, FAT_FILE *file, uint64_t bytes, FAT *f);