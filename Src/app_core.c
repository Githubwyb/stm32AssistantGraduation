#include "app_core.h"

#include "cmsis_os.h"

#include "types.h"
#include "usart.h"
#include "log.h"

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

static CORE_MAP core_msgMap[] =
{
    {CORE_USART1_RECV,               core_usart1Idle_IT},
    {CORE_USART2_RECV,               core_usart2Idle_IT},
    {CORE_USART3_RECV,               core_usart3Idle_IT},
    {CORE_USART4_RECV,               core_usart4Idle_IT},
    {CORE_USART5_RECV,               core_usart5Idle_IT},
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
