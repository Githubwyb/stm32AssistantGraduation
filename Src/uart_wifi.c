/*
 * uart_wifi.c
 *
 *  Created on: 2018年5月24日
 *      Author: wyb
 */
#include "uart_wifi.h"

#include <stdio.h>
#include <string.h>

#include "log.h"
#include "data.h"
#include "msg.h"
#include "usart.h"
#include "app_modem.h"

#define MAX_CMD_LENGTH (16)
#define MAX_CMD_NUMBER  (32)

typedef int (*WIFI_CMD_ACTION)(const unsigned char *cmdString, unsigned short length);

typedef struct
{
    unsigned char cmd[MAX_CMD_LENGTH];
    WIFI_CMD_ACTION  action;
} WIFI_CMD_MAP;

static int wifi_cmd_AT(const unsigned char *cmdString, unsigned short length)
{
    usart_print(&huart3, "OK\r\n");
    return F_SUCCESS;
}

static int wifi_cmd_ID(const unsigned char *cmdString, unsigned short length)
{
    SETTING *pSetting = NULL;
    pSetting = data_getSetting();

    usart_print(&huart3, "%s\r\n", pSetting->id);
    return F_SUCCESS;
}

static int wifi_cmd_start(const unsigned char *cmdString, unsigned short length)
{
    if(L_FALSE == data_isStart())
    {
        msg_sendCmd(APP_MODEM, MEASURE_START);
    }
    return F_SUCCESS;
}

static int wifi_cmd_end(const unsigned char *cmdString, unsigned short length)
{
    modem_clearQueue();
    data_setIsStart(L_FALSE);
    return F_SUCCESS;
}

static int wifi_cmd_set(const unsigned char *cmdString, unsigned short length)
{
    int rc = 0;
    int temp[4] = {0};
    float temp1 = 0;
    u8 needSave = L_FALSE;
    SETTING *pSetting = NULL;

    if (L_TRUE == data_isStart())
    {
        LOG_DEBUG("fpga is busy");
        uart_wifi_sendSetFailed();
        return F_FAILED;
    }

    pSetting = data_getSetting();

    rc = sscanf((char *)cmdString, "set startFre:%u endFre:%u mode:%u sum:%u dataR: %f", temp, temp + 1, temp + 2, temp + 3, &temp1);
    if (rc == 5)
    {
        if (pSetting->startFre != temp[0])
        {
            pSetting->startFre = temp[0];
            needSave = L_TRUE;
        }

        if (pSetting->endFre != temp[1])
        {
            pSetting->endFre = temp[1];
            needSave = L_TRUE;
        }

        if (pSetting->mode != temp[2])
        {
            pSetting->mode = temp[2];
            needSave = L_TRUE;
        }

        if (pSetting->sum != temp[3])
        {
            pSetting->sum = temp[3];
            needSave = L_TRUE;
        }

        if (pSetting->R != temp1)
        {
            pSetting->R = temp1;
            needSave = L_TRUE;
        }

        if (L_TRUE == needSave)
        {
            data_saveSetting();
            LOG_DEBUG("setting changed");
        }
        uart_wifi_sendSetOk();
        return F_SUCCESS;
    }

    LOG_ERROR("cmd error, rc: %d", rc);
    uart_wifi_sendSetFailed();
    return F_FAILED;
}

static int wifi_cmd_get(const unsigned char *cmdString, unsigned short length)
{
    int rc = 0;
    int temp = 0;
    DATA *pData = data_getData();
    SETTING *pSetting = data_getSetting();

    rc = sscanf((char *)cmdString, "get index: %d", &temp);
    if (rc == 1)
    {
        temp = pSetting->currentIndex - temp + 1;
        if (temp <= 0)
        {
            temp += 10;
        }
        pData->getIndex = temp;

        msg_sendCmd(APP_MODEM, GET_MEASURE_DATA);
        return F_SUCCESS;
    }

    return F_FAILED;
}

static WIFI_CMD_MAP wifi_cmd_map[] =
{
    {"AT",          wifi_cmd_AT},
    {"at",          wifi_cmd_AT},
    {"ID",          wifi_cmd_ID},
    {"id",          wifi_cmd_ID},
    {"start",       wifi_cmd_start},
    {"end",         wifi_cmd_end},
    {"set",         wifi_cmd_set},
    {"get",         wifi_cmd_get},
};

int uart_wifi_proc(const unsigned char *cmdString, unsigned short length)
{
    int i = 0;

    const unsigned char *cmd = cmdString;

    for (i = 0; i < sizeof(wifi_cmd_map) / sizeof(wifi_cmd_map[0]) && wifi_cmd_map[i].action; i++)
    {
        if (strncmp((const char *)cmd, (const char *)wifi_cmd_map[i].cmd, strlen((const char *)wifi_cmd_map[i].cmd)) == 0)
        {
            return wifi_cmd_map[i].action(cmdString, length);
        }
    }

    LOG_ERROR("wifi CMD not processed");

    return F_FAILED;
}

int uart_wifi_sendData(u8 *data, u16 length)
{
    LOG_DEBUG("wifi send data:");
    LOG_HEX(data, length);
    usart_transmit_data(&huart4, data, length);
    usart_transmit_data(&huart2, data, length);
    return F_SUCCESS;
}

int uart_wifi_sendResult(FPGA_RESULT result)
{
    char str[150] = {0};

    sprintf(str, "{\"start\":\"data\",\"RRe\":%f,\"RIm\":%f,\"fre\":%f,\"end\":\"end\"}\n",
                result.R.real,
                result.R.imagin,
                result.fre / 10.0);

    uart_wifi_sendData((u8 *)str, strlen(str));
    return F_SUCCESS;
}

int uart_wifi_sendSetting(SETTING *pSetting)
{
    char str[150] = {0};

    sprintf(str, "{\"start\":\"setting\",\"startFre\":%u,\"endFre\":%u,\"mode\":%u,\"sum\":%u,\"dataR\":%f,\"end\":\"end\"}\n",
            pSetting->startFre,
            pSetting->endFre,
            pSetting->mode,
            pSetting->sum,
            pSetting->R);

    uart_wifi_sendData((u8 *)str, strlen(str));
    return F_SUCCESS;
}

int uart_wifi_sendEnd(void)
{
    char str[] = "{\"start\":\"finished\",\"end\":\"end\"}\n";
    uart_wifi_sendData((u8 *)str, strlen(str));
    return F_SUCCESS;
}

int uart_wifi_sendSetOk(void)
{
    char str[] = "{\"set\":\"ok\"}\n";
    uart_wifi_sendData((u8 *)str, strlen(str));
    return F_SUCCESS;
}

int uart_wifi_sendSetFailed(void)
{
    char str[] = "{\"set\":\"failed\"}\n";
    uart_wifi_sendData((u8 *)str, strlen(str));
    return F_SUCCESS;
}
