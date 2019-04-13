#include "flash.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "log.h"

#define SPI_CS_ENABLE HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define SPI_CS_DISABLE HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)

extern SPI_HandleTypeDef hspi1;
extern IWDG_HandleTypeDef hiwdg;

int flash_init(void)
{
    u8 sendData[] = {RESET_ENABLE_CMD, RESET_MEMORY_CMD};
    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    SPI_CS_DISABLE;

    LOG_DEBUG("flash init");

    return F_SUCCESS;
}

int flash_isBusy(void)
{
    u8 status = 0;
    u8 sendData[] = {READ_STATUS_1};
    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    HAL_SPI_Receive(&hspi1, &status, 1, 50);
    SPI_CS_DISABLE;

    if ((status & W25Q128FV_FSR_BUSY) != 0x00)
    {
        return L_TRUE;
    }
    else
    {
        return L_FALSE;
    }
}

int flash_writeEnable(void)
{
    u8 sendData[] = {WRITE_ENABLE};
    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    SPI_CS_DISABLE;
    while(L_TRUE == flash_isBusy()){}

    return F_SUCCESS;
}

int flash_writeDisable(void)
{
    u8 sendData[] = {WRITE_DISABLE};
    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    SPI_CS_DISABLE;

    while(L_TRUE == flash_isBusy()){}

    return F_SUCCESS;
}

int flash_readID(u8 *ID)
{
    u8 sendData[] = {DEVICE_ID, 0x00, 0x00, 0x00};
    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    HAL_SPI_Receive(&hspi1, ID, 2, 50);
    SPI_CS_DISABLE;
    return F_SUCCESS;
}

int flash_read(u32 address, void *data, u16 length)
{
    u8 sendData[] = {READ_DATA, 0x00, 0x00, 0x00};
    u8 *pData = 0;

    pData = (u8 *)data;

    sendData[1] = (address >> 16) & 0xff;
    sendData[2] = (address >> 8) & 0xff;
    sendData[3] = address & 0xff;

    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    if(HAL_OK != HAL_SPI_Receive(&hspi1, pData, length, 1000))
    {
        LOG_ERROR("flash read error");
        return F_FAILED;
    }
    SPI_CS_DISABLE;

    LOG_BIN(sendData, sizeof(sendData));

    return F_SUCCESS;
}

int flash_write(u32 address, void *data, u16 length)
{
    u8 sendData[] = {PAGE_PROGRAM, 0x00, 0x00, 0x00};
    u16 dataLen = 0;
    u8 *pData = 0;

    pData = (u8 *)data;

    do
    {
        dataLen = ((address & 0xffff00) + 0x100) - address;
        if (dataLen > length)
        {
            dataLen = length;
        }

        sendData[1] = (address >> 16) & 0xff;
        sendData[2] = (address >> 8) & 0xff;
        sendData[3] = address & 0xff;

        flash_writeEnable();

        SPI_CS_ENABLE;
        HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
        if(HAL_OK != HAL_SPI_Transmit(&hspi1, pData, dataLen, 1000))
        {
            LOG_ERROR("flash write error");
            return F_FAILED;
        }
        SPI_CS_DISABLE;
        flash_writeDisable();
        while(L_TRUE == flash_isBusy()){}

        LOG_BIN(sendData, sizeof(sendData));

        address &= 0xffff00;
        address += 0x100;
        length -= dataLen;
        pData += dataLen;
    } while(length);

    return F_SUCCESS;
}

int flash_eraseSector(u32 address)
{
    u8 sendData[] = {SECTOR_ERASE, 0x00, 0x00, 0x00};
    sendData[1] = (address >> 16) & 0xff;
    sendData[2] = (address >> 8) & 0xff;
    sendData[3] = address & 0xff;

    LOG_BIN(sendData, sizeof(sendData));

    flash_writeEnable();

    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    SPI_CS_DISABLE;

    while(L_TRUE == flash_isBusy())
    {
        osDelay(10);
        HAL_IWDG_Refresh(&hiwdg);
    }

    flash_writeDisable();

    return F_SUCCESS;
}

int flash_eraseBlock_32k(u32 address)
{
    u8 sendData[] = {BLOCK_ERASE_32K, 0x00, 0x00, 0x00};
    sendData[1] = (address >> 16) & 0xff;
    sendData[2] = (address >> 8) & 0xff;
    sendData[3] = address & 0xff;

    LOG_BIN(sendData, sizeof(sendData));

    flash_writeEnable();

    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    SPI_CS_DISABLE;

    while(L_TRUE == flash_isBusy())
    {
        osDelay(10);
        HAL_IWDG_Refresh(&hiwdg);
    }

    flash_writeDisable();

    return F_SUCCESS;
}

int flash_eraseBlock_64k(u32 address)
{
    u8 sendData[] = {BLOCK_ERASE_64K, 0x00, 0x00, 0x00};
    sendData[1] = (address >> 16) & 0xff;
    sendData[2] = (address >> 8) & 0xff;
    sendData[3] = address & 0xff;

    LOG_BIN(sendData, sizeof(sendData));

    flash_writeEnable();

    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    SPI_CS_DISABLE;

    while(L_TRUE == flash_isBusy())
    {
        osDelay(10);
        HAL_IWDG_Refresh(&hiwdg);
    }

    flash_writeDisable();

    return F_SUCCESS;
}

int flash_eraseChip(void)
{
    u8 sendData[] = {CHIP_ERASE};

    LOG_BIN(sendData, sizeof(sendData));

    flash_writeEnable();

    SPI_CS_ENABLE;
    HAL_SPI_Transmit(&hspi1, sendData, sizeof(sendData), 50);
    SPI_CS_DISABLE;

    while(L_TRUE == flash_isBusy())
    {
        osDelay(10);
        HAL_IWDG_Refresh(&hiwdg);
    }

    flash_writeDisable();

    return F_SUCCESS;
}

int flash_eraseDataBlock(u32 address, u8 num)
{
    int i = 0;

    for (i = 0; i < num; ++i)
    {
        if (F_SUCCESS != flash_eraseSector(address + i * 4096))
        {
            LOG_DEBUG("erase address %p error", address + i * 4096);
            return F_FAILED;
        }
    }

    return F_SUCCESS;
}
