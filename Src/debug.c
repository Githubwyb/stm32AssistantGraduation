/*
 * debug.c
 *
 *  Created on: 2018年5月21日
 *      Author: wyb
 */
#include <stdio.h>
#include <string.h>

#include "stm32f1xx_hal.h"

#include "debug.h"
#include "log.h"
#include "flash.h"
#include "data.h"
#include "algorithm.h"
#include "uart_fpga.h"
#include "msg.h"
#include "timer.h"
#include "led.h"
#include "spi_fpga.h"
#include "spi_dds.h"

#define MAX_CMD_LENGTH (16)
#define MAX_CMD_NUMBER  (32)

typedef int (*CMD_ACTION)(const unsigned char* cmdString, unsigned short length);

typedef struct
{
    unsigned char cmd[MAX_CMD_LENGTH];
    CMD_ACTION  action;
} CMD_MAP;

#define DBG_OUT(fmt, ...) log_print("[DEBUG]>"fmt, ##__VA_ARGS__);log_print("\r\n")

extern IWDG_HandleTypeDef hiwdg;

static int cmd_debug(const unsigned char* cmdString, unsigned short length);

static int cmd_AT(const unsigned char* cmdString, unsigned short length)
{
    LOG_DEBUG("OK");
    return F_SUCCESS;
}

static int cmd_flash(const unsigned char *cmdString, unsigned short length)
{
    int rc = 0;
    u32 address = 0;
    u8 data[65] = {0};
    u32 dataLen = 0;

    rc = sscanf((char *)cmdString, "flash write address: %d data: %s", &address, data);
    if (rc == 2)
    {
        dataLen = strlen((char *)data);
        flash_write(address, data, dataLen);
        LOG_DEBUG("write to address: %p", address);
        LOG_HEX((char *)data, dataLen);
        return F_SUCCESS;
    }

    // LOG_DEBUG("rc %d", rc);
    // LOG_HEX(cmdString, length);

    rc = sscanf((char *)cmdString, "flash read address: %d length: %d", &address, &dataLen);
    if (rc == 2)
    {
        if (dataLen > 64)
        {
            dataLen = 64;
            LOG_ERROR("data length is too long, max is 64");
        }
        LOG_DEBUG("read %d bytes from address: %p", dataLen, address);
        flash_read(address, data, dataLen);
        LOG_HEX((char *)data, dataLen);
        return F_SUCCESS;
    }

    rc = sscanf((char *)cmdString, "flash erase 4k address: %d", &address);
    if (rc == 1)
    {
        address &= 0xfff000;
        LOG_DEBUG("erase 4k address: 0x%p - 0x%p", address, address + 0x1000);
        flash_eraseSector(address);
        return F_SUCCESS;
    }

    rc = sscanf((char *)cmdString, "flash erase 32k address: %d", &address);
    if (rc == 1)
    {
        address &= 0xfff000;
        LOG_DEBUG("erase 32k address: 0x%p - 0x%p", address, address + 0x8000);
        flash_eraseBlock_32k(address);
        return F_SUCCESS;
    }

    rc = sscanf((char *)cmdString, "flash erase 64k address: %d", &address);
    if (rc == 1)
    {
        address &= 0xfff000;
        LOG_DEBUG("erase 64k address: 0x%p - 0x%p", address, address + 0x10000);
        flash_eraseBlock_64k(address);
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "flash erase all", strlen("flash erase all")) == 0)
    {
        flash_eraseChip();
        LOG_DEBUG("flash erase all chip");
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "flash id", strlen("flash id")) == 0)
    {
        flash_readID(data);
        LOG_DEBUG("flash id m: %d d: %d", data[0], data[1]);
        return F_SUCCESS;
    }

    LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_data(const unsigned char *cmdString, unsigned short length)
{
    DATA *pData = NULL;

    pData = data_getData();

    if (strncmp((const char *)cmdString, "data reset", strlen("data reset")) == 0)
    {
        data_initData();
        LOG_DEBUG("reset data");
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "data recievedFlag", strlen("data recievedFlag")) == 0)
    {
        LOG_DEBUG("data recievedFlag %d", pData->recievedFlag);
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "data index", strlen("data index")) == 0)
    {
        LOG_DEBUG("data index %d", pData->getIndex);
        return F_SUCCESS;
    }

    LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_setting(const unsigned char *cmdString, unsigned short length)
{
    int rc = 0;
    int temp = 0;
    int i = 0;
    int cmdLen = 0;
    SETTING *pSetting = NULL;

    pSetting = data_getSetting();

    rc = sscanf((char *)cmdString, "setting set sum %d", &temp);
    if (rc == 1)
    {
        if (L_TRUE == data_isStart())
        {
            LOG_DEBUG("fpga is busy");
            // uart_wifi_sendSetFailed();
            return F_FAILED;
        }
        if (pSetting->sum != temp)
        {
            pSetting->sum = temp;
            data_saveSetting();
        }
        return F_SUCCESS;
    }

    rc = sscanf((char *)cmdString, "setting set mode %d", &temp);
    if (rc == 1)
    {
        if (L_TRUE == data_isStart())
        {
            LOG_DEBUG("fpga is busy");
            // uart_wifi_sendSetFailed();
            return F_FAILED;
        }
        if (pSetting->mode != temp)
        {
            pSetting->mode = temp;
            data_saveSetting();
        }
        return F_SUCCESS;
    }

    rc = sscanf((char *)cmdString, "setting set averageNum %d", &temp);
    if (rc == 1)
    {
        if (L_TRUE == data_isStart())
        {
            LOG_DEBUG("fpga is busy");
            // uart_wifi_sendSetFailed();
            return F_FAILED;
        }
        if (pSetting->averageNum != temp)
        {
            pSetting->averageNum = temp;
            data_saveSetting();
        }
        return F_SUCCESS;
    }

    rc = sscanf((char *)cmdString, "setting set ignoreNum %d", &temp);
    if (rc == 1)
    {
        if (L_TRUE == data_isStart())
        {
            LOG_DEBUG("fpga is busy");
            // uart_wifi_sendSetFailed();
            return F_FAILED;
        }
        if (pSetting->ignoreNum != temp)
        {
            pSetting->ignoreNum = temp;
            data_saveSetting();
        }
        return F_SUCCESS;
    }

    cmdLen = strlen("setting set id");
    if (strncmp((const char *)cmdString, "setting set id", cmdLen) == 0
        && length > cmdLen)
    {
        int idLen = length - cmdLen - 1;
        if (idLen > MAX_ID_SIZE)
        {
            idLen = MAX_ID_SIZE;
        }

        memcpy(pSetting->id, cmdString + cmdLen + 1, idLen);
        pSetting->id[idLen] = 0x00;
        data_saveSetting();
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "setting save", strlen("setting save")) == 0)
    {
        data_saveSetting();
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "setting load", strlen("setting load")) == 0)
    {
        if (L_TRUE == data_isStart())
        {
            LOG_DEBUG("fpga is busy");
            return F_FAILED;
        }
        data_loadSetting();
        data_showSetting();
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "setting show", strlen("setting show")) == 0)
    {
        data_showSetting();
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "setting reset", strlen("setting reset")) == 0)
    {
        if (L_TRUE == data_isStart())
        {
            LOG_DEBUG("fpga is busy");
            return F_FAILED;
        }
        flash_eraseChip();
        pSetting->ignoreNum = 0x03;
        pSetting->averageNum = 0x03;
        pSetting->sum = 1001;
        pSetting->mode = FPGA_MODE_LINE;
        pSetting->currentIndex = 0x00;
        pSetting->haveMeasuredNum = 0;
        pSetting->R = 50;
        pSetting->startFre = 10;
        pSetting->endFre = 1000000;

        pSetting->blockAddress[0] = BLOCK_START_ADDRESS;
        pSetting->blockSize[0] = 0;

        for (i = 1; i < BLOCK_NUM; ++i)
        {
            pSetting->blockAddress[i] = BLOCK_START_ADDRESS + BLOCK_MAX_SIZE + BLOCK_DATA_SIZE * (i - 1);
            pSetting->blockSize[i] = 0;
        }

        pSetting->blockSize[0] = sizeof(SETTING);
        memcpy(pSetting->id, "QYDZ", sizeof("QYDZ"));
        pSetting->id[sizeof("QYDZ")] = 0;

        data_saveSetting();
        data_showSetting();
        led_end();
        return F_SUCCESS;
    }

    LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_start(const unsigned char *cmdString, unsigned short length)
{
    int rc = 0;
    u32 fre = 0;

    DATA *pData = NULL;

    pData = data_getData();

    rc = sscanf((char *)cmdString, "start frequency %d", &fre);
    if (rc == 1)
    {
        if (L_TRUE == data_isStart())
        {
            LOG_ERROR("fpga is busy");
            return F_FAILED;
        }
        pData->curFre = fre;
        msg_sendCmd(APP_MODEM, MEASURE_ONCE);
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "start frequency all", strlen("start frequency all")) == 0)
    {
        msg_sendCmd(APP_MODEM, MEASURE_START);
        return F_SUCCESS;
    }

    LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_stop(const unsigned char *cmdString, unsigned short length)
{
    data_setIsStart(L_FALSE);
    return F_SUCCESS;
}

static int cmd_get(const unsigned char *cmdString, unsigned short length)
{
    int rc = 0;
    int temp = 0;
    DATA *pData = NULL;

    pData = data_getData();

    rc = sscanf((char *)cmdString, "get index %d", &temp);
    if (rc == 1)
    {
        pData->getIndex = temp;
        msg_sendCmd(APP_MODEM, GET_MEASURE_DATA);
        return F_SUCCESS;
    }

    if (length == 3)
    {
        msg_sendCmd(APP_MODEM, GET_MEASURE_DATA);
        return F_SUCCESS;
    }

    LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_size(const unsigned char *cmdString, unsigned short length)
{
    if (strncmp((const char *)cmdString, "size result", strlen("size result")) == 0)
    {
        LOG_DEBUG("sizeof result is %d", sizeof(FPGA_RESULT));
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "size data", strlen("size data")) == 0)
    {
        LOG_DEBUG("sizeof data is %d", sizeof(DATA));
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "size setting", strlen("size setting")) == 0)
    {
        LOG_DEBUG("sizeof setting is %d", sizeof(SETTING));
        return F_SUCCESS;
    }

    LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_led(const unsigned char *cmdString, unsigned short length)
{
    int rc = 0;
    int temp = 0;
    if (strncmp((const char *)cmdString, "led blink", strlen("led blink")) == 0)
    {
        LOG_DEBUG("led blink");
        led_start();
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "led eliminate", strlen("led eliminate")) == 0)
    {
        LOG_DEBUG("led eliminate");
        led_end();
        return F_SUCCESS;
    }

    rc = sscanf((char *)cmdString, "led write %d", &temp);
    if (rc == 1)
    {
        LOG_DEBUG("led write to %d", temp);
        leds_write((u8)temp);
        return F_SUCCESS;
    }

    LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_dog(const unsigned char *cmdString, unsigned short length)
{
    if (strncmp((const char *)cmdString, "dog feed", strlen("dog feed")) == 0)
    {
        LOG_DEBUG("feed dog");
        HAL_IWDG_Refresh(&hiwdg);
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "dog hungry", strlen("dog hungry")) == 0)
    {
        LOG_DEBUG("let the dog hungry forever");
        timer_stop(TIMER_WATCHDOG);
        return F_SUCCESS;
    }

    if (strncmp((const char *)cmdString, "dog kill", strlen("dog kill")) == 0)
    {
        LOG_DEBUG("I don't like the dog, let's kill it");
        LOG_ERROR("dog is too strong, I can't kill it");
        return F_FAILED;
    }

    LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_fpga(const unsigned char *cmdString, unsigned short length)
{
    // if (strncmp((const char *)cmdString, "fpga send data: ", strlen("fpga send data: ")) == 0)
    // {
    //     fpga_sendData((u8 *)(cmdString + 15), length - 15);
    //     PRINT("send to fpga: :");
    //     LOG_BIN((u8 *)(cmdString + 15), length - 15);
    //     return F_SUCCESS;
    // }

    // LOG_ERROR("CMD error");
    return F_FAILED;
}

static int cmd_dds(const unsigned char *cmdString, unsigned short length)
{
    // int rc = 0;
    // int temp = 0;

    // rc = sscanf((char *)cmdString, "dds set frequency %d", &temp);
    // if (rc == 1)
    // {
    //     dds_waveSeting(temp, 0, SIN_WAVE, 0);
    //     LOG_DEBUG("dds set frequency to %d", temp);
    //     return F_SUCCESS;
    // }

    // LOG_ERROR("CMD error");
    return F_FAILED;
}

static CMD_MAP cmd_map[] =
{
        {"debug",       cmd_debug},
        {"AT",          cmd_AT},
        {"at",          cmd_AT},
        {"flash",       cmd_flash},
        {"data",        cmd_data},
        {"setting",     cmd_setting},
        {"start",       cmd_start},
        {"stop",        cmd_stop},
        {"get",         cmd_get},
        {"size",        cmd_size},
        {"led",         cmd_led},
        {"dog",         cmd_dog},
        {"fpga",        cmd_fpga},
        {"dds",         cmd_dds},
};


static int cmd_debug(const unsigned char* cmdString, unsigned short length)
{
    int i = 0;
    DBG_OUT("support cmd:");
    for (i = 0; i < sizeof(cmd_map) / sizeof(cmd_map[0]) && cmd_map[i].action; i++)
    {
        DBG_OUT("\t%s\t%p", cmd_map[i].cmd, cmd_map[i].action);
    }

    return F_SUCCESS;
}

int debug_proc(const unsigned char *cmdString, unsigned short length)
{
    int i = 0;

    const unsigned char *cmd = cmdString;

    for (i = 0; i < sizeof(cmd_map) / sizeof(cmd_map[0]) && cmd_map[i].action; i++)
    {
        if (strncmp((const char *)cmd, (const char *)cmd_map[i].cmd, strlen((const char *)cmd_map[i].cmd)) == 0)
        {
            return cmd_map[i].action(cmdString, length);
        }
    }

    LOG_INFO("CMD not processed");

    return F_FAILED;
}
