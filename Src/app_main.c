#include <stdlib.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "app_main.h"
#include "app_modem.h"
#include "log.h"
#include "usart.h"
#include "msg.h"
#include "key.h"
#include "debug.h"
#include "flash.h"
#include "data.h"
#include "version.h"
#include "uart_fpga.h"
#include "uart_wifi.h"
#include "led.h"
#include "timer.h"
#include "spi_dds.h"
#include "spi_fpga.h"

extern osMessageQId mainQueueHandle;
extern IWDG_HandleTypeDef hiwdg;

static int main_usart1Recv(const TASK_MSG *msg)
{
    LOG_DEBUG("usart1 recieve %d bytes, data: %s", msg->length, msg->data);
#ifdef DEBUG
    debug_proc(msg->data, msg->length);
#endif
    return F_SUCCESS;
}

static int main_usart2Recv(const TASK_MSG *msg)
{
    LOG_DEBUG("bluetooth recieve %d bytes, data: ", msg->length);
    LOG_HEX(msg->data, msg->length);
    uart_wifi_proc(msg->data, msg->length);

    return F_SUCCESS;
}

static int main_usart3Recv(const TASK_MSG *msg)
{
    LOG_DEBUG("usart3 recieve %d bytes", msg->length);
    // LOG_HEX((char *)msg->data, msg->length);
    // uart_fpga_input(msg->data, msg->length);
    return F_SUCCESS;
}

static int main_usart4Recv(const TASK_MSG *msg)
{
    LOG_DEBUG("wifi recieve %d bytes", msg->length);
    LOG_HEX((char *)msg->data, msg->length);
    uart_wifi_proc(msg->data, msg->length);
    return F_SUCCESS;
}

static int main_usart5Recv(const TASK_MSG *msg)
{
    LOG_DEBUG("usart5 recieve %d bytes", msg->length);
    // LOG_HEX((char *)msg->data, msg->length);
    // uart_fpga_output(msg->data, msg->length);
    return F_SUCCESS;
}

static int main_spi1Recv(const TASK_MSG *msg)
{
    // LOG_DEBUG("Spi fpga recv");
    parsSpiFpgaData(msg->data);
    return F_SUCCESS;
}

static int main_spi3Recv(const TASK_MSG *msg)
{
    // LOG_DEBUG("Spi dds recv");
    parsSpiDdsData();
    return F_SUCCESS;
}

static int main_gpioExitRising0(const TASK_MSG *msg)
{
    // LOG_DEBUG("Fpga START_1");
    msg_sendCmd(APP_MODEM, START_SEND);
    return F_SUCCESS;
}

static int main_gpioExitFalling0(const TASK_MSG *msg)
{
    // LOG_DEBUG("Fpga START_0");
    return F_SUCCESS;
}

static int main_gpioExitRising1(const TASK_MSG *msg)
{
    // LOG_DEBUG("Fpga RST_1");
    return F_SUCCESS;
}

static int main_gpioExitFalling1(const TASK_MSG *msg)
{
    // LOG_DEBUG("Fpga RST_0");
    return F_SUCCESS;
}

static int main_keyChange(const TASK_MSG *msg) {
    const KEY_MSG *key_msg = (const KEY_MSG *)msg->data;

    LOG_DEBUG("Key %d changed to %d", key_msg->key, key_msg->state);

    return F_SUCCESS;
}

static int main_feedDog(const TASK_MSG *msg)
{
    // LOG_DEBUG("feed dog, wang!!!");
    // spi_fpga_printStatus();
    HAL_IWDG_Refresh(&hiwdg);
    return F_SUCCESS;
}

static TASK_MSG_MAP main_msgMap[] =
{
    {USART1_RECV,               main_usart1Recv},
    {USART2_RECV,               main_usart2Recv},
    {USART3_RECV,               main_usart3Recv},
    {USART4_RECV,               main_usart4Recv},
    {USART5_RECV,               main_usart5Recv},
    {SPI1_RECV,                 main_spi1Recv},
    {SPI3_RECV,                 main_spi3Recv},
    {GPIO_EXIT_RISING_0,        main_gpioExitRising0},
    {GPIO_EXIT_FALLING_0,       main_gpioExitFalling0},
    {GPIO_EXIT_RISING_1,        main_gpioExitRising1},
    {GPIO_EXIT_FALLING_1,       main_gpioExitFalling1},
    {KEY_CHANGE,                main_keyChange},
    {FEED_DOG,                  main_feedDog},
};

int stm32_init(void)
{
    LOG_DEBUG("stm32f103zet6 version: "VERSION_STR);
    usart_init();
    return F_SUCCESS;
}

static int watchdogTimerCb(void)
{
    msg_sendCmd(APP_MAIN, FEED_DOG);
    timer_start(TIMER_WATCHDOG, 1000, watchdogTimerCb);
    return F_SUCCESS;
}

void app_main(void const * argument)
{
    TASK_MSG *pMsg = NULL;
    int i = 0;

    //初始化硬件
    LOG_INFO("Init hardware");
    led_checkStart();
    // dds_waveSeting(0, 0, SIN_WAVE, 0);
    //定时开启看门狗
    timer_start(TIMER_WATCHDOG, 1000, watchdogTimerCb);
    //检查flash情况
    for (i = 0; i < FLASH_RETRY_TIMES; ++i) {
        if (F_SUCCESS == data_loadSetting()) {
            led_checkEnd();
            break;
        }
        HAL_IWDG_Refresh(&hiwdg);
        osDelay(FLASH_RETRY_INTERVAL);
    }

    if (i == FLASH_RETRY_TIMES) {
        LOG_WARN("Flash init failed, maybe it's the first time to use this machine");
    } else {
        data_showSetting();
    }

    //初始化软件
    LOG_INFO("Init software");
    data_initData();

    LOG_DEBUG("task main start");
    spi_fpga_recvStart();
    spi_dds_recvStart();
    srand(xTaskGetTickCount());
    while(1)
    {
        xQueueReceive(mainQueueHandle, &pMsg, portMAX_DELAY);
        msg_msgProc(pMsg, main_msgMap, sizeof(main_msgMap) / sizeof(main_msgMap[0]));
    }
}
