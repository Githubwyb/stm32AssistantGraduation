#include "msg.h"

#include <string.h>
#include "cmsis_os.h"

#include "log.h"

extern osMessageQId mainQueueHandle;
extern osMessageQId coreQueueHandle;
extern osMessageQId modemQueueHandle;

int msg_sendCmd(TASK_ENUM task, TASK_MSG_ENUM cmd)
{
    TASK_MSG *pMsg = (TASK_MSG *)pvPortMalloc(sizeof(TASK_MSG));
    if(pMsg == NULL)
    {
        LOG_ERROR("msg malloc failed");
        return F_FAILED;
    }

    pMsg->cmd = cmd;
    pMsg->length = 0;

    // LOG_DEBUG("send cmd (%d) to task (%d)", cmd, task);

    switch(task)
    {
        case APP_MAIN:
            return xQueueSend(mainQueueHandle, &pMsg, 0);

        case APP_MODEM:
            return xQueueSend(modemQueueHandle, &pMsg, 0);

        default:
            return F_FAILED;
    }
}

int msg_sendData(TASK_ENUM task, TASK_MSG_ENUM cmd, void *data, u8 length)
{
    TASK_MSG *pMsg = (TASK_MSG *)pvPortMalloc(sizeof(TASK_MSG) + length + 1);

    if(pMsg == NULL)
    {
        LOG_ERROR("msg malloc failed");
        return F_FAILED;
    }

    pMsg->cmd = cmd;
    pMsg->length = length;
    memcpy(pMsg->data, data, length);
    pMsg->data[length] = 0;

    // LOG_DEBUG("send cmd (%d) to task (%d)", cmd, task);

    switch(task)
    {
        case APP_MAIN:
            return xQueueSend(mainQueueHandle, &pMsg, 0);

        case APP_MODEM:
            return xQueueSend(modemQueueHandle, &pMsg, 0);

        default:
            return F_FAILED;
    }
}

int msg_sendCoreCmd(CORE_MSG_ENUM cmd)
{
    u8 msg = cmd;

    return xQueueSendFromISR(coreQueueHandle, &msg, 0);
}

int msg_msgProc(TASK_MSG *msg, const TASK_MSG_MAP *pMsgMap, int count)
{
    int i = 0;
    int rc = F_SUCCESS;

    for(i = 0; i < count; i++)
    {
        if(msg->cmd == pMsgMap[i].cmd)
        {
            if(pMsgMap[i].pFun)
            {
                rc = pMsgMap[i].pFun(msg);
                vPortFree(msg);
                return rc;
            }
            else
            {
                LOG_DEBUG("cmd %d has no handler", msg->cmd);
                vPortFree(msg);
                return F_FAILED;
            }
        }
    }

    LOG_ERROR("unknown cmd %d", msg->cmd);
    vPortFree(msg);
    return F_FAILED;
}
