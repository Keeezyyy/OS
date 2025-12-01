#include "./fat.h"

inline uint64_t getFatStart(FAT *f)
{

    return f->bootSector.reservedSectors * f->bootSector.bytesPerSector;
}
inline uint64_t getRootStart(FAT *f)
{
    // ReservedSectors + (NumFATs * FATsize_in_sectors)
    //
    return (f->bootSector.reservedSectors + (f->bootSector.numberOfFATs * f->bootSector.sectorsPerFAT)) * f->bootSector.bytesPerSector;
}

inline uint64_t getDataStart(FAT *f)
{
    uint64_t RootDirSectors =
        (f->bootSector.maxRootDirEntries * 32 + (f->bootSector.bytesPerSector - 1)) / f->bootSector.bytesPerSector;

    uint64_t RootDirBytes = RootDirSectors * f->bootSector.bytesPerSector;

    uint64_t FATRegionBytes =
        (f->bootSector.reservedSectors + f->bootSector.numberOfFATs * f->bootSector.sectorsPerFAT) * f->bootSector.bytesPerSector;

    return FATRegionBytes + RootDirBytes;
}

bool fat_init(FAT *f)
{
    if (!save_disk_read((uint16_t *)(&f->bootSector), BOOTSECTOR_LBA))
    {
        return false;
    }

    f->fatStartInBytes = getFatStart(f);
    f->rootStartInBytes = getRootStart(f);
    f->dataStartInBytes = getDataStart(f);

    // //printf("Fat Start : 0x%x\n", getFatStart(f));
    // //printf("Root Start : 0x%x\n", getRootStart(f));
    // //printf("Data Start : 0x%x\n", getDataStart(f));

    return true;
}

void to_fat_name(const char *input, char out[11])
{
    memset(out, ' ', 11);

    const char *dot = strchr(input, '.');
    int i = 0;

    while (input[i] != '\0' && input[i] != '.' && i < 8)
    {
        out[i] = toupper(input[i]);
        i++;
    }

    if (dot != 0x0)
    {
        dot++;
        int e = 0;
        while (dot[e] != '\0' && e < 3)
        {
            out[8 + e] = toupper(dot[e]);
            e++;
        }
    }
}

void printFatName(const char const *str)
{
    for (int i = 0; i < 11; i++)
        putc(*(str + i));
}

bool open(void *buffer, const char *path, FAT_FILE *out, FAT **out_FAT)
{
    // setting up path
    char *p = path;
    if (*p == '/')
        p++;
    char fileName[11];

    // inizilizing the FAT obj
    FAT f;
    fat_init(&f);

    memcpy((void *)0x1000, &f, sizeof(FAT));

    // reading first root entries
    FAT_DirEntry dirEntries[16];

    // searching for file in entries
    FAT_DirEntry entry;
    entry.DIR_Attr = 0xFF;
    int count = 0;

    bool foundDirOrFile = false;

    bool fileFound = false;
    for (int i = 0; i < f.bootSector.maxRootDirEntries / 16; i++)
    {

        uint32_t lba = (f.rootStartInBytes + 511) / 512;
        if (!save_disk_read((uint16_t *)(dirEntries), lba + i))
            return false;

        char *slash = strchr(p, '/');

        // is i nroot directory
        if (slash != 0x0)
            *slash = '\0';

        to_fat_name(p, fileName);

        memset(&entry, 0, sizeof(FAT_DirEntry));

        fat_findFileInDir(&f, (uint8_t *)dirEntries, &entry, fileName);

        if (entry.DIR_Attr != 0x0)
        {
            // //printf("file or dir fileFound in root\n");
            // //printf("fileFound in cluster num : 0x%x\n", entry.DIR_FstClusLO);
            // printf("atribute : 0x%x\n", entry.DIR_Attr);
            p = slash + 1;
            foundDirOrFile = true;
            if (entry.DIR_Attr != 0x10)
                fileFound = true;
            break;
        }
    }

    if (!foundDirOrFile)
        halt();

    bool morePath = true;
    bool foundDir = true;
    uint32_t clusterNum = entry.DIR_FstClusLO;

    while (!fileFound && morePath && foundDir)
    {

        char *slash = strchr(p, '/');
        if (slash != 0x0)
        {
            *slash = '\0';
            morePath = true;
        }
        else
        {
            morePath = false;
        }
        clusterNum = entry.DIR_FstClusLO - 2;

        to_fat_name(p, fileName);

        memset(&entry, 0, sizeof(FAT_DirEntry));

        // printf("searching in sectors in cluster: 0x%x\n", f.bootSector.sectorsPerCluster);

        foundDir = false;

        for (int i = 0; i < f.bootSector.sectorsPerCluster; i++)
        {
            uint32_t lba = (f.dataStartInBytes + (clusterNum * f.bootSector.bytesPerSector * f.bootSector.sectorsPerCluster) + 511) /
                           512;
            lba += i;

            if (!save_disk_read((uint16_t *)(dirEntries), lba))
                return false;

            if (fat_findFileInDir(&f, (uint8_t *)dirEntries, &entry, fileName))
            {
                // printf("starts in cluster : 0x%x\n", entry.DIR_FstClusLO);
                foundDir = true;

                if (entry.DIR_Attr == 0x10)
                {

                    if (slash != 0x0)
                        p = slash + 1;

                    clusterNum = entry.DIR_FstClusLO - 2;
                }
                else
                {
                    clusterNum = entry.DIR_FstClusLO;
                    i = 0xFFFFFF;
                    fileFound = true;
                }

                break;
            }
        }

        if (!foundDir)
            break;
    }

    if (fileFound)
    {
        printf("starts in cluster : 0x%x\n", entry.DIR_FstClusLO);
        out->clusterStartNum = clusterNum;
        out->size = entry.DIR_FileSize;
        *out_FAT = (FAT *)0x1000;

        printFatName(entry.DIR_Name);
        printf("datei gefunden.\n");
    }
    else
    {
        printf("Nichts gefunden.\n");
    }
}

bool fat_findFileInDir(FAT *f, uint8_t *bBlock, FAT_DirEntry *out, const char *const name)
{
    for (int i = 0; i < 32; i++)
    {
        FAT_DirEntry *tmp_entry = (FAT_DirEntry *)(uint8_t *)(bBlock + i * 32);
        if (memcmp(tmp_entry->DIR_Name, name, 11))
        {
             printf("Datei gefunden!!   : \"");
             printFatName(tmp_entry->DIR_Name);
             printf("\" \n");
            // printf("starts in cluster : 0x%x\n", tmp_entry->DIR_FstClusLO);
            //  printHexDump(&tmp_entry, 10);

            memcpy(out, tmp_entry, 32);

            return true;
        }
    }
    return false;
}
uint32_t getNextClusterNum(uint32_t currentClusterNum, FAT *f)
{
    uint16_t *buffer = 0x0000;
    memset(buffer, 0, sizeof(buffer));

    uint32_t fatOffset = currentClusterNum * 2;
    uint32_t sectorOffset = fatOffset / 512;
    uint32_t offsetInSector = fatOffset % 512;

    uint32_t lba = f->fatStartInBytes / f->bootSector.bytesPerSector + sectorOffset;

    //printf("lba: 0x%x\n", lba);

    if (!save_disk_read(buffer, lba))
        return 0xFFFFFFFF;

    printHexDump(buffer, 2);

    uint16_t next = *(uint16_t *)((uint8_t *)(buffer + offsetInSector / 2));

    printf("Next cluster  : 0x%x\n", next);
    //printf("Next cluster at   : 0x%x\n", (uint32_t)((uint8_t *)(buffer + offsetInSector / 2)));
    //printf("buffer   : 0x%x\n", buffer);
    //printf("offsetInSector   : 0x%x\n", offsetInSector);

    if (next > 0x19)
    {
    }

    return next;
}

bool readFile(void *buffer, FAT_FILE *file, uint64_t bytes, FAT *f)
{
    uint64_t count = 0;

    uint64_t clusterNum = file->clusterStartNum;

    do
    {
        // 2 * 512 ->2 reserved in fat table
        uint32_t lba = (f->dataStartInBytes + (clusterNum * f->bootSector.bytesPerSector * f->bootSector.sectorsPerCluster) + 511) / 512;

        count++;

        for (int i = 0; i < f->bootSector.sectorsPerCluster; i++)
        {
            if (!save_disk_read((uint16_t *)(buffer) + 256 * i, lba + i - 2 * f->bootSector.sectorsPerCluster))
                    return false;
        }

        buffer = (uint8_t *)buffer + f->bootSector.sectorsPerCluster * 512;

        if (clusterNum < 0xFFF6)
            clusterNum = getNextClusterNum(clusterNum, f);

        // printf("next cluster num : 0x%x\n", clusterNum);

    } while (clusterNum < 0xFFF6 && clusterNum > 2);

    printf("sectors copied = 0x%x\n", count);
    printf("sould copy = 0x%x\n", file->size / f->bootSector.bytesPerSector / f->bootSector.sectorsPerCluster);

    // if the num of sectors matches the amount of copied sectors
    return file->size / f->bootSector.sectorsPerCluster / f->bootSector.bytesPerSector <= count;
}