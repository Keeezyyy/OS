#include "./fat.h"

inline uint64_t getFatStart(FAT *f)
{
    // return ReservedSectors * BytesPerSector
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
    if (!disk_read((uint16_t *)(&f->bootSector), BOOTSECTOR_LBA, sizeof(FAT16_BootSector)))
    {
        return false;
    }

    f->fatStartInBytes = getFatStart(f);
    f->rootStartInBytes = getRootStart(f);
    f->dataStartInBytes = getDataStart(f);

    // printf("Fat Start : 0x%x\n", getFatStart(f));
    // printf("Root Start : 0x%x\n", getRootStart(f));
    // printf("Data Start : 0x%x\n", getDataStart(f));

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

bool open(void *buffer, const char *path, FAT_FILE *out)
{
    // setting up path
    char *p = path;
    if (*p == '/')
        p++;
    char fileName[11];

    // inizilizing the FAT obj
    FAT f;
    fat_init(&f);

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
        if (!disk_read((uint16_t *)(dirEntries), lba + i, 512))
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
            // printf("file or dir fileFound in root\n");
            // printf("fileFound in cluster num : 0x%x\n", entry.DIR_FstClusLO);
            // printf("atribute : 0x%x\n", entry.DIR_Attr);
            p = slash + 1;
            foundDirOrFile = true;
            if(entry.DIR_Attr != 0x10)
                fileFound = true;
            break;
        }
    }

    if(!foundDirOrFile)
        halt();

    bool morePath = true;
    bool foundDir = true;
    uint32_t clusterNum = entry.DIR_FstClusLO;

    while (!fileFound && morePath && foundDir)
    {
        printf("\n");

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

        printf("searching in sectors in cluster: 0x%x\n", f.bootSector.sectorsPerCluster);

        foundDir = false;

        for (int i = 0; i < f.bootSector.sectorsPerCluster; i++)
        {
            uint32_t lba = (f.dataStartInBytes +(clusterNum * f.bootSector.bytesPerSector * f.bootSector.sectorsPerCluster) + 511) /
                           512;
            lba += i;

            if (!disk_read((uint16_t *)(dirEntries), lba, 512))
                return false;

            if (fat_findFileInDir(&f, (uint8_t *)dirEntries, &entry, fileName))
            {

                foundDir = true;

                if (entry.DIR_Attr == 0x10)
                {

                    if (slash != 0x0)
                        p = slash + 1;

                    clusterNum = entry.DIR_FstClusLO - 2;
                }
                else
                {

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
        out->clusterStartNum = clusterNum;
        printf("Datei gefunden.\n");

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
            // printHexDump(&tmp_entry, 10);

            memcpy(out, tmp_entry, 32);

            return true;
        }
    }
    return false;
}