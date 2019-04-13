#ifndef _TIMER_H_
#define _TIMER_H_

#include "types.h"

#define TIMER_UART1_TIMEOUT TIMER_1
#define TIMER_UART2_TIMEOUT TIMER_2
#define TIMER_UART3_TIMEOUT TIMER_3
#define TIMER_UART4_TIMEOUT TIMER_4
#define TIMER_UART5_TIMEOUT TIMER_5
#define TIMER_DELAY TIMER_6
#define TIMER_WATCHDOG TIMER_7
#define TIMER_LED_BUZZER TIMER_8
#define TIMER_FPGA_TIMEOUT TIMER_9

typedef int (*TIMER_CB)(void);

typedef struct
{
    u8 isStart;
    u32 count;
    u32 period;
    TIMER_CB pFun;
} TIMER_STRUCT;

void timer1_cb(void const * argument);
int timer_update_IT(void);
int timer_start(TIMER_ENUM id, u32 period, TIMER_CB pFun);
int timer_stop(TIMER_ENUM id);
int timer_isTimerStart(TIMER_ENUM id);
int timer_delay_ms(u32 count);

#endif // !_TIMER_H_
