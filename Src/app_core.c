#include "app_core.h"

#include "cmsis_os.h"

#include "types.h"
#include "usart.h"
#include "log.h"
#include "msg.h"
#include "spi_dds.h"
#include "spi_fpga.h"

typedef int (*CORE_PROC)(void);

typedef struct
{
    CORE_MSG_ENUM cmd;
    CORE_PROC pFun;
} CORE_MAP;

static int core_usart1Idle_IT(void)
{
    usart1_recvData_IT();
    return F_SUCCESS;
}

static int core_usart2Idle_IT(void)
{
    usart2_recvData_IT();
    return F_SUCCESS;
}

static int core_usart3Idle_IT(void)
{
    usart3_recvData_IT();
    return F_SUCCESS;
}

static int core_usart4Idle_IT(void)
{
    usart4_recvData_IT();
    return F_SUCCESS;
}

static int core_usart5Idle_IT(void)
{
    usart5_recvData_IT();
    return F_SUCCESS;
}

static int core_gpioExitRising0_IT(void)
{
    msg_sendCmd(APP_MAIN, GPIO_EXIT_RISING_0);
    return F_SUCCESS;
}

static int core_gpioExitFalling0_IT(void)
{
    msg_sendCmd(APP_MAIN, GPIO_EXIT_FALLING_0);
    return F_SUCCESS;
}

static int core_gpioExitRising1_IT(void)
{
    msg_sendCmd(APP_MAIN, GPIO_EXIT_RISING_1);
    return F_SUCCESS;
}

static int core_gpioExitFalling1_IT(void)
{
    msg_sendCmd(APP_MAIN, GPIO_EXIT_FALLING_1);
    return F_SUCCESS;
}

static int core_spi1Recv_IT(void)
{
    spi_fpgaRecv_IT();
    return F_SUCCESS;
}

static int core_spi3Recv_IT(void)
{
    // LOG_DEBUG("Spi3 recv");
    msg_sendCmd(APP_MAIN, SPI3_RECV);
    spi_dds_recvStart();
    return F_SUCCESS;
}

static CORE_MAP core_msgMap[] =
{
    {CORE_USART1_RECV,              core_usart1Idle_IT},
    {CORE_USART2_RECV,              core_usart2Idle_IT},
    {CORE_USART3_RECV,              core_usart3Idle_IT},
    {CORE_USART4_RECV,              core_usart4Idle_IT},
    {CORE_USART5_RECV,              core_usart5Idle_IT},
    {CORE_GPIO_EXIT_RISING_0,       core_gpioExitRising0_IT},
    {CORE_GPIO_EXIT_FALLING_0,      core_gpioExitFalling0_IT},
    {CORE_GPIO_EXIT_RISING_1,       core_gpioExitRising1_IT},
    {CORE_GPIO_EXIT_FALLING_1,      core_gpioExitFalling1_IT},
    {CORE_SPI1_RECV,                core_spi1Recv_IT},
    {CORE_SPI3_RECV,                core_spi3Recv_IT},
};

static int core_msgHandler(u8 msg)
{
    int i = 0;

    for(i = 0; i < sizeof(core_msgMap) / sizeof(core_msgMap[0]); i++)
    {
        if(msg == core_msgMap[i].cmd)
        {
            if(core_msgMap[i].pFun)
            {
                return core_msgMap[i].pFun();
            }
            else
            {
                LOG_DEBUG("core msg (%d) has no handler");
                return F_SUCCESS;
            }
        }
    }

    LOG_ERROR("unknown core msg (%d)");

    return F_FAILED;
}

extern osMessageQId coreQueueHandle;

void app_core(void const * argument)
{
    u8 msg = 0;

    LOG_DEBUG("task core start");

    while(1)
    {
        xQueueReceive(coreQueueHandle, &msg, portMAX_DELAY);
        core_msgHandler(msg);
    }
}
