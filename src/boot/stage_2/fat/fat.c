#include "./fat.h"

bool fat_init(FAT *f)
{
    if (!disk_read((uint16_t *)(&f->bootSector), BOOTSECTOR_LBA, sizeof(FAT16_BootSector)))
    {
        return false;
    }

    f->fatStart = f->bootSector.reservedSectors * f->bootSector.bytesPerSector;

    // printf("fat start : 0x%x\n", f->fatStart);

    f->numOfFats = f->bootSector.numberOfFATs;
    // printf("num of fats : 0x%x\n", f->numOfFats);

    uint64_t fatSize = f->bootSector.sectorsPerFAT * f->bootSector.bytesPerSector * f->numOfFats;

    // printf("fat size in bytes: 0x%x\n", fatSize);
    f->rootStart = fatSize + f->fatStart;

    // printf("root start: 0x%x\n", f->rootStart);
    return true;
}

bool fat_findFileInRoot(FAT *f, FAT_DirEntry *out, const char *const name, const char *const ext)
{

    bool found = false;
    uint32_t blocksTested = 0;
    while (!found || f->bootSector.maxRootDirEntries != blocksTested)
    {
        uint16_t lba = f->rootStart / 512;
        if (!disk_read((uint16_t *)(&f->readBufferBlock), lba, sizeof(BUFFER_BLOCK)))
            return false;

        FAT_DirEntry *tmp_entry = (FAT_DirEntry *)(&f->readBufferBlock);

        for (int i = 0; i < 32; i++)
        {
            //if(memcmp(tmp_entry->DIR_Name, ))
            tmp_entry++;
        }

        blocksTested++;
    }
    return true;
}