#include "./disk.h"

bool read(uint16_t *buffer_adr, uint64_t LBA, uint16_t sectorCount)
{
    uint8_t DiskState = read_io_byte(STATUS_COMMAND);
    // if (DiskState & 0b10000000 || !(DiskState & 0b00100000))
    //     return false;

    write_io_byte(SECTOR_COUNT, sectorCount);

    write_io_byte(LBA_LOW, LBA & 0xFF);
    write_io_byte(LBA_MID, (LBA >> 8) & 0xFF);
    write_io_byte(LBA_HIGH, (LBA >> 16) & 0xFF);
    write_io_byte(DRIVE_HEAD, 0b11100000 | ((LBA >> 24) & 0b00011111));

    write_io_byte(STATUS_COMMAND, DISK_COMMAND_READ_SECTOR);

    while (read_io_byte(STATUS_COMMAND) & 0b10000000 == 0)
    {
    }

    for (int i = 0; i < sectorCount * 256; i++)
    {
        *buffer_adr = read_io_word(0x1F0);
        buffer_adr++;
    }
    return true;
}

/*
bool read(void *buffer_adr, uint64_t LBA, uint16_t sectorCount)
{
    write_io_byte(DRIVE_HEAD, 0b11100000 | ((LBA >> 24) & 0b00011111));
    write_io_byte(SECTOR_COUNT, sectorCount);

    uint8_t DiskState = read_io_byte(STATUS_COMMAND);

    if (DiskState & 0b10000000 || !(DiskState & 0b00100000))
        return false;


    write_io_byte(LBA_LOW, LBA & 0xFF);
    write_io_byte(LBA_MID, (LBA >> 8) & 0xFF);
    write_io_byte(LBA_HIGH, (LBA >> 16) & 0xFF);


    write_io_byte(STATUS_COMMAND, DISK_COMMAND_READ_SECTOR);

    while (!(read_io_byte(STATUS_COMMAND) & 0b10000000) && read_io_byte(STATUS_COMMAND) & 0b00001000 && !(read_io_byte(STATUS_COMMAND) & 0b00000001))
    {

    }

}*/