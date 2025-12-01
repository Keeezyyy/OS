#include "./disk.h"

bool disk_read(uint16_t *buffer_adr, uint64_t LBA)
{

    


    uint8_t LBA1 = (LBA >> 0) & 0xFF;
    uint8_t LBA2 = (LBA >> 8) & 0xFF;
    uint8_t LBA3 = (LBA >> 16) & 0xFF;
    uint8_t LBA4 = (LBA >> 24) & 0xFF;
    uint8_t LBA5 = (LBA >> 32) & 0xFF;
    uint8_t LBA6 = (LBA >> 40) & 0xFF;

    uint16_t sectors = 1;

    write_io_byte(DRIVE_HEAD, 0x40);

    write_io_byte(SECTOR_COUNT, (sectors >> 8) & 0xFF);
    write_io_byte(LBA_LOW, (LBA >> 24) & 0xFF);
    write_io_byte(LBA_MID, (LBA >> 32) & 0xFF);
    write_io_byte(LBA_HIGH, (LBA >> 40) & 0xFF);

    write_io_byte(SECTOR_COUNT, sectors & 0xFF);
    write_io_byte(LBA_LOW, (LBA >> 0) & 0xFF);
    write_io_byte(LBA_MID, (LBA >> 8) & 0xFF);
    write_io_byte(LBA_HIGH, (LBA >> 16) & 0xFF);

    write_io_byte(STATUS_COMMAND, 0x24);

    while (read_io_byte(STATUS_COMMAND) & 0x80)
        ;

    uint8_t status = read_io_byte(STATUS_COMMAND);

    if (!(status & 0x08))
        return false;

    uint16_t *adr = buffer_adr;
    for (int i = 0; i < (uint16_t)(512 / 2); i++)
    {
        *adr = read_io_word(0x1F0);
        adr++;
    }
    int x = 0;
    for(int i = 0; i<512;i++){
        x = read_io_word(0x1F0);
    }

    while (read_io_byte(STATUS_COMMAND) & DISK_STATUS_DATA_LEFT_IN_CACHE)
        read_io_word(0x1F0);

    disk_flush();

    return true;
}

void disk_flush()
{
    write_io_byte(STATUS_COMMAND, 0xE7);
}

bool save_disk_read(uint16_t *buffer_adr, uint64_t LBA)
{
    //printf("buffer adr, 0x%x,    lba: 0x%x,   bytecount : 0x1%x  \n", buffer_adr, LBA);
    bool worked = false;
    for (int i = 0; i < DISK_RETRY_AMOUNT; i++)
    {
        if(disk_read(buffer_adr, LBA)){
            worked = true;
            break;
        }
    }

    if(!worked)
        printf("!!!! disk 5 times retry error \n");

    return worked;
}