#ifndef _MSG_H_
#define _MSG_H_

#include "types.h"

int msg_sendCmd(TASK_ENUM task, TASK_MSG_ENUM cmd);
int msg_sendData(TASK_ENUM task, TASK_MSG_ENUM cmd, void *data, u8 length);
int msg_sendCoreCmd(CORE_MSG_ENUM cmd);
int msg_msgProc(TASK_MSG *msg, const TASK_MSG_MAP *pMsgMap, int count);

#endif // !_MSG_H_
