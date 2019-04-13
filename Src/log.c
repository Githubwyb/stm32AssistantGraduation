/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.h"

#include "cmsis_os.h"

#include "types.h"
#include "usart.h"

#include <time.h>
#include <string.h>
#include <stdarg.h>

/*
 * @function 获得level的打印名
 * @param level level级别
 */
static const char *getLevelString(LOG_LEVEL level);

/*
 * @function 打印当前时间
 */
static void print_currentTime(void);

/*
 * @function 获取当前task名称
 */
static u8 *get_currentTaskName(void);

void log_print(const char *fmt, ...)
{
    va_list arg;
    u16 length = 0;
    u8 data[UART_TX_BUF_LEN] = {0};

    va_start(arg, fmt);
    vsnprintf((char *)data, UART_TX_BUF_LEN, fmt, arg);
    va_end(arg);

    length = strlen((const char *)data);

    usart_transmit_data(&huart1, data, length);
}

/*
 * The hex log is in the following format:
 *
 *     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F      0123456789ABCDEF
 * 01  aa 55 01 00 00 00 25 00 38 36 35 30 36 37 30 32     .U....%.86506702
 * 02  30 34 39 30 31 36 38 30 00 00 00 00 00 00 00 00     04901680........
 * 03  00 00 00 00 00 00 00 00 00 00 00 00                 ............
 *
 */
void log_hex(const void *data, int length)
{
    int i = 0, j = 0;
    const char *pData = (const char *)data;

    log_print("    ");
    for (i = 0; i < 16; i++)
    {
        log_print("%X  ", i);
    }
    log_print("    ");
    for (i = 0; i < 16; i++)
    {
        log_print("%X", i);
    }

    log_print("\r\n");

    for (i = 0; i < length; i += 16)
    {
        log_print("%02d  ", i / 16 + 1);
        for (j = i; j < i + 16 && j < length; j++)
        {
            log_print("%02x ", pData[j] & 0xff);
        }
        if (j == length && length % 16)
        {
            for (j = 0; j < (16 - length % 16); j++)
            {
                log_print("   ");
            }
        }
        log_print("    ");
        for (j = i; j < i + 16 && j < length; j++)
        {
            if (pData[j] < 32 || pData[j] >= 127)
            {
                log_print(".");
            }
            else
            {
                log_print("%c", pData[j] & 0xff);
            }
        }

        log_print("\r\n");
    }
}

/*
 * 打印二进制数据接口
 * 当前仅支持打印前32个字节的二进制数据
 */
#define MAX_PRINTED_BIN_LENGTH (32)

void log_binary(const void *data, int length)
{
    const char *point = NULL;
    int i = 0;

    point = (const char *)data;

    for (i = 0; i < length && i < MAX_PRINTED_BIN_LENGTH; i++)
    {
        log_print("%02x ", point[i]);
    }

    if (i >= MAX_PRINTED_BIN_LENGTH)
    {
        log_print("......");
    }

    log_print("\r\n");
}

void log_header(LOG_LEVEL level)
{
    print_currentTime();
    log_print(" [%s][%s]", get_currentTaskName(), getLevelString(level));
}

const char *splitFileName(const char *fileName)
{
    const char *pChar = fileName;
    pChar = (strrchr(pChar, '/') ? strrchr(pChar, '/') + 1 : (strrchr(pChar, '\\') ? strrchr(pChar, '\\') + 1 : pChar));
    return pChar;
}

const char *getLevelString(LOG_LEVEL level)
{
    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        return "D";

    case LOG_LEVEL_WARN:
        return "W";

    case LOG_LEVEL_INFO:
        return "I";

    case LOG_LEVEL_ERROR:
        return "E";

    case LOG_LEVEL_FATAL:
        return "F";

    default:
        return "";
    }
}

void print_currentTime(void)
{
    time_t currentTime = xTaskGetTickCount() / 1000;
    struct tm *currentTm = localtime(&currentTime);
    log_print("%02d:%02d:%02d",
              currentTm->tm_hour,
              currentTm->tm_min,
              currentTm->tm_sec);
}

u8 *get_currentTaskName(void)
{
    TaskHandle_t taskCurrentHandle = xTaskGetCurrentTaskHandle();
    if (taskCurrentHandle != NULL)
    {
        return (u8 *)pcTaskGetName(taskCurrentHandle);
    }

    return (u8 *)("os");
}
