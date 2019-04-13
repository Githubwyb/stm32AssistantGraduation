#ifndef _FLASH_H_
#define _FLASH_H_

#include "types.h"

typedef enum
{
    WRITE_STATUS = 0x01,
    PAGE_PROGRAM = 0x02,
    READ_DATA = 0x03,
    WRITE_DISABLE = 0x04,
    READ_STATUS_1 = 0x05,
    WRITE_ENABLE = 0x06,
    SECTOR_ERASE = 0x20,
    READ_STATUS_2 = 0x35,
    BLOCK_ERASE_32K = 0x52,
    BLOCK_ERASE_64K = 0xd8,
    CHIP_ERASE = 0x60,
    RESET_ENABLE_CMD = 0x66,
    DEVICE_ID = 0x90,
    RESET_MEMORY_CMD = 0x99,
} FLASH_CMD;

/* Flag Status Register */
#define W25Q128FV_FSR_BUSY ((uint8_t)0x01) /*!< busy */
#define W25Q128FV_FSR_WREN ((uint8_t)0x02) /*!< write enable */
#define W25Q128FV_FSR_QE ((uint8_t)0x02)   /*!< quad enable */

int flash_init(void);
int flash_readID(u8 *ID);
int flash_read(u32 address, void *data, u16 length);
int flash_write(u32 address, void *data, u16 length);
int flash_eraseSector(u32 address);
int flash_eraseBlock_32k(u32 address);
int flash_eraseBlock_64k(u32 address);
int flash_eraseChip(void);
int flash_eraseDataBlock(u32 address, u8 num);

#endif // !_FLASH_H_
